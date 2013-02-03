#ifndef CTRL_DEP_H_CHOPPER
#define CTRL_DEP_H_CHOPPER 

#include <llvm/Pass.h>
#include <llvm/Function.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/Analysis/Dominators.h>

#include <vector>
#include <utility>
#include <set>

using namespace llvm;

namespace chopper {
    /** 
     * Calculates control dependence in every function.
     */ 
    class CtrlDep : public FunctionPass {
    public:
        friend class CtrlDepWriter;

        typedef std::pair<BasicBlock*, BasicBlock*> BBEdge;
        typedef struct {
            size_t id;
            size_t depth;
        } BBInfo;
        typedef llvm::DenseMap<BasicBlock*, size_t> BBMap;
        typedef std::vector<
            std::pair<BasicBlock*, BBInfo> 
        > BBList;

        typedef llvm::DenseMap<
            BasicBlock*,
            std::set<BasicBlock*>
        > CtrlDepMap;

        typedef struct {
            llvm::StringRef func;
            CtrlDepMap cdMap;
            BBMap bbMap;
        } CtrlDepInfo;

        static char ID;

        CtrlDep ();

        virtual bool runOnFunction(Function&);

        virtual void getAnalysisUsage(AnalysisUsage&) const;

        virtual bool doFinalization(Module&);

        virtual ~CtrlDep ();
    
    private:
        /**
         * Perform an euler tour around post dominator tree
         * to downgrade the LCA to RMQ problem.
         */
        void traversePdt(DomTreeNode*, BBMap&, BBList&, size_t);

        size_t seqCounter;

        std::vector<CtrlDepInfo> cds;

    };
} /* chopper */

#endif
