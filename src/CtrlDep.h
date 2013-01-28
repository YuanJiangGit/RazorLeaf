#ifndef CTRL_DEP_H_CHOPPER
#define CTRL_DEP_H_CHOPPER value

#include <llvm/Pass.h>
#include <llvm/Function.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/Analysis/Dominators.h>

#include <vector>
#include <utility>

using namespace llvm;

namespace chopper {
    /** 
     * Calculates control dependence in every function.
     */ 
    class CtrlDep : public FunctionPass {
    public:
        typedef std::pair<BasicBlock*, BasicBlock*> BBEdge;
        typedef struct {
            size_t id;
            size_t depth;
        } BBInfo;
        typedef llvm::DenseMap<BasicBlock*, size_t> BBMap;
        typedef std::vector<
            std::pair<BasicBlock*, BBInfo> 
        > BBList;

        static char ID;

        CtrlDep ();

        virtual bool runOnFunction(Function&);

        virtual void getAnalysisUsage(AnalysisUsage&) const;

        virtual ~CtrlDep ();
    
    private:
        /**
         * Perform an euler tour around post dominator tree
         * to downgrade the LCA to RMQ problem.
         */
        void traversePdt(DomTreeNode*, BBMap&, BBList&, size_t);

        size_t seqCounter;

    };
} /* chopper */

#endif

