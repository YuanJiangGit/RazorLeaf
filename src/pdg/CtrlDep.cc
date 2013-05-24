#include "CtrlDep.h"

#include <llvm/Support/CFG.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ValueSymbolTable.h>
#include <llvm/Instructions.h>

#include <utility>
#include <cassert>
#include <cmath>

using namespace llvm;
using namespace razorleaf;

CDG::CDG()
{
}

CDG::~CDG()
{
}

void
CDG::addEdge(BasicBlock *source,
        BasicBlock *target)
{
    unsigned int sid = 0, tid = 0;
    for (; sid<vertexSet.size(); sid++) {
        if (source == vertexSet[sid].bb) {
            break;
        }
    }

    if (sid == vertexSet.size()) {
        Vertex v = {source, AdjList()};
        vertexSet.push_back(v);
    }

    for (; tid<vertexSet.size(); tid++) {
        if (target == vertexSet[tid].bb) {
            break;
        }
    }

    if (tid == vertexSet.size()) {
        Vertex v = {target, AdjList()};
        vertexSet.push_back(v);
    }

    vertexSet[sid].adjList.push_back(tid);
}

CtrlDep::CtrlDep(Function *f, PostDominatorTree *p):
    func(f), pdt(p), cdg(new CDG())
{
    findAllEdges();
    traversePDT(pdt->getRootNode(), 0);
    tarjanOLCA(pdt->getRootNode());
}

void
CtrlDep::traversePDT(DomTreeNode *root, DomTreeNode *father)
{
    BBInfo bbInfo = {0, 0, father, 0, 0};
    dtnMap.insert(std::make_pair(root, bbInfo));
    for (DomTreeNodeBase<BasicBlock>::iterator iter = root->begin();
            iter != root->end(); ++iter) {
        traversePDT(*iter, root);
    }
}

void
CtrlDep::tarjanOLCA(DomTreeNode *root)
{
    dtnMap[root].ancestor = root;
    dtnMap[root].rank = 0;

    for (DomTreeNodeBase<BasicBlock>::iterator iter = root->begin();
            iter != root->end(); ++iter) {
        tarjanOLCA(*iter);
        unionSet(root, *iter);
        DomTreeNode *ancestor = find(root);
        dtnMap[ancestor].ancestor = root;
    }

    dtnMap[root].isMarked = true;

    for (BBEdge &edge : edgeList) {
        if (edge.isMarked) {
            continue;
        }
        if (root->getBlock() == edge.first || 
                root->getBlock() == edge.second) {
            DomTreeNode *v;
            if (root->getBlock() == edge.first) {
                v = pdt->operator[](edge.second);
            } else {
                v = pdt->operator[](edge.first);
            }
            if (dtnMap.count(v) && dtnMap[v].isMarked) {
                BasicBlock *lca = dtnMap[find(v)].ancestor->getBlock();
                /*
                errs() << "lca of \"" << edge.first->getName()
                    << "\" and \"" << edge.second->getName()
                    << "\" :\n";
                    */
                edge.isMarked = true;
                if (lca) {
                    //errs() << lca->getName() << ".\n";
                }
                if (lca == edge.first) {
                    cdg->addEdge(edge.first, edge.first);
                }
                if (v->getBlock() != edge.second) {
                    v = pdt->operator[](edge.second);
                }
                DomTreeNode *dtn = v;

                do {
                    cdg->addEdge(edge.first, dtn->getBlock());
                    dtn = dtnMap[dtn].father;
                } while (dtn && dtn->getBlock() != lca);
                
            }
        }     
    }
}

void 
CtrlDep::unionSet(DomTreeNode *x, DomTreeNode *y)
{
    DomTreeNode *xRoot = find(x);
    DomTreeNode *yRoot = find(y);

    int xRank = dtnMap[xRoot].rank;
    int yRank = dtnMap[yRoot].rank;

    if (xRank > yRank) {
        dtnMap[yRoot].parent = xRoot;
    } else if (xRank < yRank) {
        dtnMap[xRoot].parent = yRoot;
    } else if (xRoot != yRoot) {
        dtnMap[yRoot].parent = xRoot;
        dtnMap[xRoot].rank += 1;
    }
}

DomTreeNode *
CtrlDep::find(DomTreeNode *x)
{
    DomTreeNode *parent = dtnMap[x].parent;
    if (parent == x) {
        return x;
    } else {
        dtnMap[x].parent = find(parent);
        return dtnMap[x].parent;
    }
}

void 
CtrlDep::findAllEdges() {
    /* find all edges */
    for (BasicBlock &bb : func->getBasicBlockList()) {
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
                if (!pdt->dominates(sb, &bb)) {
                    BBEdge edge = {&bb, sb, false};
                    edgeList.push_back(edge);
                }
            }
        }
    }
}

CtrlDep::~CtrlDep()
{
    delete cdg;
}

CDG *
CtrlDep::getCDG() {
   return cdg;
}

