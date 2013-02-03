#define DEBUG_TYPE "CtrlDep"

#include "CtrlDep.h"
#include "CtrlDepWriter.h"

#include <llvm/Support/CFG.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/ValueSymbolTable.h>
#include <llvm/Instructions.h>

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>

#ifdef _DEBUG
#include <llvm/Support/Debug.h>
#else
#define DEBUG_WITH_TYPE(X, Y) {}
#endif

#include <utility>
#include <cassert>
#include <cmath>

using namespace llvm;
using namespace chopper;
using namespace log4cxx;

static LoggerPtr logger(Logger::getLogger("chopper"));

CtrlDep::CtrlDep():FunctionPass(CtrlDep::ID), 
    seqCounter(0)
{
PropertyConfigurator::configure("log4cxx.properties");
}

CtrlDep::~CtrlDep()
{
}

/**
* 1. traverse the cfg, store all basic block info
*    to a map. O(N)
* 2. traverse the pdt, store the post domination info
*    in the map. O(N)
* 3. find valid edges and store them to the edge set.
*    O(N)
* 4. find the lowset(least) common ancestor of all 
*    edges in the edge set and thus retrieve control 
*    dependence info. O(Nlog(N) + N)
*/
bool 
CtrlDep::runOnFunction(Function &F) {
    LOG4CXX_DEBUG(logger, "CtrlDep in Func " 
            << F.getName().data());

    PostDominatorTree &pdt = getAnalysis<PostDominatorTree>();
    std::vector<BBEdge> edgeList;

    /* find all edge */
    for (BasicBlock &bb : F.getBasicBlockList()) {
        TerminatorInst *termInst = bb.getTerminator();
    
        bool isEdge = false;
        BranchInst *bInst; 
        SwitchInst *sInst;
        if ((bInst = dyn_cast<BranchInst>(termInst))) {
            if (bInst->isConditional()) {
                isEdge = true;
            } else {
                //TODO to handle unconditional junp instructions 
            }
        } else if ((sInst = dyn_cast<SwitchInst>(termInst))) {
            isEdge = true;
        }
        
        if (isEdge) {
            for (unsigned i=0; i<termInst->getNumSuccessors(); i++) {
                BasicBlock *sb = termInst->getSuccessor(i);
                if (!pdt.dominates(sb, &bb)) {
                    edgeList.push_back(std::make_pair(&bb, sb));
                }
            }
        }
        
    }

    /* process post dominator tree */
    DomTreeNode *root = pdt.getRootNode();

    BBMap bbMap;
    BBList bbList;
    CtrlDepMap cdMap;
    this->seqCounter = 0;

    traversePdt(root, bbMap, bbList, 0);

    LOG4CXX_DEBUG(logger, "Traversed PDT.\n");
    for (std::pair<BasicBlock*, BBInfo> bb : bbList) {
        LOG4CXX_DEBUG(logger, "id." << bb.second.id
                << " depth:" << bb.second.depth);
    }

    /**
     * Computing LCA for each candidate edge.
     */
    size_t blockSize = static_cast<size_t>(
            .5*log(bbList.size())/log(2.0f));
    size_t blockCount = bbList.size()/blockSize + 1;

    // The assertion should never fail unless
    // number of basic block in PDT. causes integer overflow.
    assert(blockSize <= 32);

    if (blockSize > 0) {
        // stores each block into blockType array 
        unsigned int *blockType = new unsigned int[blockCount];

        // all posible answer to RMQ with the size of
        // 2 ^ (blockSize - 1)
        unsigned int allPosible = 
            1 << (blockSize-1);
        unsigned int allCombination =
            ((blockSize-1)*blockSize) >> 1;

        unsigned int *blockIndex = 
            new unsigned int[
                allCombination *
                allPosible
            ];

        // fill the index table
        for (unsigned int i = 0; 
                i < allPosible; i++) {
            unsigned int counter = 0;
            blockIndex[i*allCombination] = 0;
            for (unsigned int pi = 0; pi < blockSize-1; pi++) {
                int tmpVal = 0;
                // first : minimum value,
                // second : minimum index
                std::pair<int, unsigned int> ist = {0, pi};
                for (unsigned int pj = pi+1; 
                        pj < blockSize; pj++, counter++) {
                    if ((1 << (pj-1)) & i) {
                        // +1
                        tmpVal += 1;
                        if (tmpVal > ist.first) {
                            ist.first = tmpVal;
                            ist.second = pj;
                        }
                    } else {
                        // -1
                        tmpVal -= 1;
                    }
                    blockIndex[i*allCombination + 
                       counter] = ist.second;
                }
            }
            
        }

        for (unsigned int i = 0; i < allCombination*allPosible; i++) {
            LOG4CXX_DEBUG(logger, "bi[" << i << "]: " << blockIndex[i]);
        }

        // fill the block type array 
        for (int i = 0; i < blockCount; i++) {
            unsigned int type = 0;
            unsigned int mark = 1;
            blockType[i] = 0;
            for (int j = 0; j < blockSize-1 && 
                    (i*blockSize+j+1) < bbList.size();
                    j++) {
                if (bbList[i*blockSize + j].second.depth
                        > bbList[i*blockSize + j+1].second.depth) {
                    type |= mark;
                } 
                mark <<= 1;
            }
            blockType[i] = type;
            LOG4CXX_DEBUG(logger, "block type " << i
                    << " -> " << type );

        }

        LOG4CXX_DEBUG(logger, "Prepared for LCA algorithm.");
        LOG4CXX_DEBUG(logger, "Block Size " << blockSize << ".");
        LOG4CXX_DEBUG(logger, "Block Count " << blockCount << ".");
        LOG4CXX_DEBUG(logger, "Block Index Capacity " 
                << allCombination * allPosible << ".");

        // STEP II:
        for (BBEdge edge : edgeList) {
            size_t eA = bbList[bbMap[edge.first]].second.id;
            size_t eB = bbList[bbMap[edge.second]].second.id;
            size_t i,j;

            if (eA > eB) {
                i = eB;
                j = eA;
            } else {
                i = eA;
                j = eB;
            }

            LOG4CXX_DEBUG(logger, "node i :" << i << 
                        ", j :" << j );

            size_t noI = i/blockSize;
            size_t noJ = j/blockSize;
            size_t offsetI = i%blockSize;
            size_t offsetJ = j%blockSize;

            // lca index
            size_t lca = 0;

            if (noI == noJ) {
               lca = noI*blockSize +
                    blockIndex[
                    blockType[noI] *
                        allCombination + 
                        ((((blockSize << 1)-offsetI-1)*offsetI) >> 1) + offsetJ - offsetI - 1
                        ];
                LOG4CXX_INFO(logger, 
                        "Condition I i :" << i << 
                        ", j :" << j << 
                        ", lca id : " << lca );
            } else {
                // from i to BlockSize - 1
                size_t leftI ;
                if (offsetI == blockSize -1 ) {
                    leftI = i;
                } else {
                    leftI = noI*blockSize +
                    blockIndex[
                    blockType[noI] *
                        allCombination + 
                        ((((blockSize << 1)-offsetI-1)*offsetI) >> 1) + blockSize - offsetI - 2
                        ];
                }
                // from 0 to J
                size_t rightJ = offsetJ == 0 ? noJ*blockSize :
                    noJ*blockSize +
                    blockIndex[blockType[noJ] * allCombination + offsetJ - 1];
                // first id
                // second depth
                std::pair<size_t, size_t> candidate;
                // first id
                if (bbList[leftI].second.depth < bbList[rightJ].second.depth) {
                    candidate.first = bbList[leftI].second.id;
                    candidate.second = bbList[leftI].second.depth;
                } else {
                    candidate.first = bbList[rightJ].second.id;
                    candidate.second = bbList[rightJ].second.depth;
                }


                for (unsigned int p = noI+1; p < noJ; p++) {
                    size_t mid = p*blockSize +
                        blockIndex[blockType[p] * allCombination + blockSize - 2];
                    if (candidate.second > bbList[mid].second.depth) {
                        candidate.first = bbList[mid].second.id;
                        candidate.second = bbList[mid].second.depth;
                        LOG4CXX_DEBUG(logger, "mid " << 
                                candidate.first << " : " 
                                << candidate.second );
                    }
                }

                lca = candidate.first;

                // STEP : insert the control dependence information into 
                // cdMap
                LOG4CXX_INFO(logger, 
                        "Condition II i :" << i << 
                        ", j :" << j << 
                        ", lca id : " << lca );
                BasicBlock *cdBB;
                cdBB = bbList[eA].first;
                size_t p;
                if (eA < eB) {
                    // i <- eA, j <- eB
                    // if lca is A, all nodes in pdt on the path A->B
                    // loop dependence
                    // including A & B, should make control dependence on A
                    // otherwise, from lca -> B (lca not included)

                    errs () << *cdBB << " and " << (*bbList[eB].first) << "\n";
                    p = lca == eA ? lca : lca + 1;
                    for (; p <= eB; p++) {
                        if (!cdMap.count(cdBB)) {
                            std::set<BasicBlock*> setref;
                            cdMap.insert(std::make_pair(cdBB, setref));
                        } 
                        cdMap[cdBB].insert(bbList[p].first);
                    }
                } else {
                    errs () << *cdBB << " & " << (*bbList[eB].first) << "\n";
                    size_t tmpDelim = lca == eA ? lca + 1 : lca;
                    for (p = eB; p < tmpDelim; p++) {
                        if (!cdMap.count(cdBB)) {
                            std::set<BasicBlock*> setref;
                            cdMap.insert(std::make_pair(cdBB, setref));
                        } 
                        cdMap[cdBB].insert(bbList[p].first);
                    }
                }

            }
        }

        delete [] blockIndex;
        delete [] blockType;

    } else {
        for (BBEdge edge : edgeList) {
            size_t eA = bbList[bbMap[edge.first]].second.id;
            size_t eB = bbList[bbMap[edge.second]].second.id;
            size_t i,j;

            if (eA > eB) {
                i = eB;
                j = eA;
            } else {
                i = eA;
                j = eB;
            }

            size_t lca = i;
            size_t depth = bbList[i].second.depth;

            // use a naiive algorithm to find RMQ in this case
            for (int p = i+1; p <= j; p++) {
                if (bbList[p].second.depth < depth) {
                    depth = bbList[p].second.depth;
                    lca = p;
                }
            }

            LOG4CXX_DEBUG(logger, 
                    "Condition III i :" << i << 
                    ", j :" << j << 
                    ", lca id : " << lca );
            BasicBlock *cdBB;
            cdBB = bbList[eA].first;

            size_t p;
            if (eA < eB) {
                // i <- eA, j <- eB
                p = lca == eA ? lca : lca + 1;
                for (; p <= eB; p++) {
                    if (!cdMap.count(cdBB)) {
                        cdMap.insert(std::make_pair(cdBB, 
                                    std::set<BasicBlock*>()));
                    } 
                    cdMap[cdBB].insert(bbList[p].first);
                }
            } else {
                size_t tmpDelim = lca == eA ? lca + 1 : lca;
                for (size_t p = eB; p < tmpDelim; p++) {
                    if (!cdMap.count(cdBB)) {
                        cdMap.insert(std::make_pair(cdBB, 
                                    std::set<BasicBlock*>()));
                    } 
                    cdMap[cdBB].insert(bbList[p].first);
                }
            }

        }
    }

    CtrlDepInfo info = {
        F.getName(),
        cdMap,
        bbMap
    };
    cds.push_back(info);
    
    return false;
}

bool 
CtrlDep::doFinalization(Module &M) {
    CtrlDepWriter::write(this);
    return false;
}

void 
CtrlDep::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<PostDominatorTree>();
}

void
CtrlDep::traversePdt(DomTreeNode* root,
        BBMap &bbMap, 
        BBList &bbList,
        size_t depth)
{
    BBInfo info = {this->seqCounter, depth};
    BasicBlock *rootBlock = root->getBlock();

    if (!bbMap.count(rootBlock)) {
        bbMap.insert(std::make_pair(rootBlock,
                    this->seqCounter));
        bbList.push_back(std::make_pair(rootBlock, info));
    } else {
        //This should never happen
        assert(false);
    }
    this->seqCounter += 1;

    /* traverse */
    if (root->getNumChildren()) {
        for (DomTreeNodeBase<BasicBlock>::iterator
                iter = root->begin(); iter != root->end();
                ++iter) {
            traversePdt(*iter, bbMap, bbList, depth+1);

            if (bbMap.count(rootBlock)) {
                info = {this->seqCounter, depth};
                bbList.push_back(std::make_pair(rootBlock, info));
            } else {
                assert(false);
            }
            this->seqCounter += 1;
        }
    }

}

char CtrlDep::ID = 0;
static RegisterPass<CtrlDep> X("ctrl-dep", 
        "control dependence pass", false, true);

