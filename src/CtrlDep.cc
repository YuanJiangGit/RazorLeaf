#define DEBUG_TYPE "CtrlDep"

#include "CtrlDep.h"

#include <llvm/Support/CFG.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/ValueSymbolTable.h>
#include <llvm/Instructions.h>

#include <utility>
#include <cassert>

using namespace llvm;
using namespace chopper;

CtrlDep::CtrlDep():FunctionPass(CtrlDep::ID), 
    seqCounter(0)
{
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
    errs() << "In Func : ";
    errs().write_escaped(F.getName()) << "\n";

    PostDominatorTree &pdt = 
        getAnalysis<PostDominatorTree>();
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
            for (unsigned i=0; i<termInst->getNumSuccessors(); ++i) {
                BasicBlock *sb = termInst->getSuccessor(i);
                if (!pdt.dominates(sb, &bb)) {
                    edgeList.push_back(std::make_pair(sb, &bb));
                }
            }
        }
        
    }

    /* process post dominator tree */
    DomTreeNode *root = pdt.getRootNode();

    BBMap bbMap;
    BBList bbList;
    this->seqCounter = 0;

    traversePdt(root, bbMap, bbList, 0);

    for (BBEdge edge : edgeList) {
    }
    /*
    errs() << "retrieving array ...";
    for (std::pair<BasicBlock*, BBInfo> p : bbList) {
        errs() << p.second.id << "," << p.second.depth << "->";
    }
    */
    
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
        }

        if (bbMap.count(rootBlock)) {
            info = {this->seqCounter, depth};
            bbList.push_back(std::make_pair(rootBlock, info));
        } else {
            assert(false);
        }
        this->seqCounter += 1;
    }

}

char CtrlDep::ID = 0;
static RegisterPass<CtrlDep> X("ctrl-dep", 
        "control dependence pass", false, true);

