#ifndef DATA_DEP_H_CHOPPER
#define DATA_DEP_H_CHOPPER 

#include <llvm/Pass.h>
#include <llvm/Function.h>
#include <llvm/Analysis/MemoryDependenceAnalysis.h>
#include <llvm/Analysis/AliasAnalysis.h>

using namespace llvm;

namespace chopper {
    class PDG;
    class PDGPass : public FunctionPass {
    public:
        static char ID;
        PDGPass ();

        virtual bool runOnFunction(Function&);

        virtual void getAnalysisUsage(AnalysisUsage&) const;

        virtual ~PDGPass ();
    private:
        PDG *buildPDG(Function&, 
                MemoryDependenceAnalysis&,
                AliasAnalysis&);
    };
} /* chopper */

#endif

