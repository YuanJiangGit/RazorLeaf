#ifndef CTRL_DEP_H_CHOPPER
#define CTRL_DEP_H_CHOPPER 

#include <llvm/Pass.h>
#include <llvm/Function.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/Analysis/Dominators.h>
#include <llvm/Analysis/PostDominators.h>

#include <vector>
#include <list>
#include <utility>
#include <set>

using namespace llvm;

using std::list;
using std::vector;

namespace chopper {
    /**
     * Control Dependence Graph.
     */
    class CDG {
    public:
        typedef list<unsigned int> AdjList;
        typedef struct {
            BasicBlock *bb;
            AdjList adjList;
        } Vertex;

        typedef vector<Vertex> VertexSet;
        typedef VertexSet::iterator iterator;

        void addEdge(BasicBlock*, BasicBlock*);

        inline iterator begin() 
        { return vertexSet.begin(); }

        inline iterator end() 
        { return vertexSet.end(); }

        CDG();
        virtual ~CDG ();
    private:
        VertexSet vertexSet;
    };

    /** 
     * Calculates control dependence in every function.
     */ 
    class CtrlDep {
    public:
        //friend class CtrlDepWriter;

        typedef struct {
            BasicBlock *first, *second;
            bool isMarked;
        } BBEdge;

        typedef struct {
            DomTreeNode *parent;
            DomTreeNode *ancestor;
            DomTreeNode *father;
            bool isMarked;
            int rank;
        } BBInfo;
        typedef llvm::DenseMap<DomTreeNode*, BBInfo> BBMap;
       
        CtrlDep (Function*, PostDominatorTree*);

        CDG* getCDG();

        virtual ~CtrlDep ();
    
    private:
        Function *func;
        PostDominatorTree *pdt;
        CDG *cdg;
        BBMap dtnMap;
        std::vector<BBEdge> edgeList;
        void findAllEdges();
        void tarjanOLCA(DomTreeNode *);
        void traversePDT(DomTreeNode *, DomTreeNode *);
        void unionSet(DomTreeNode *, DomTreeNode *);
        DomTreeNode* find(DomTreeNode *);
    };
} /* chopper */

#endif

