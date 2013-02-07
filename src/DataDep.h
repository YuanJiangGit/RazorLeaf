#ifndef DATA_DEP_H_CHOPPER
#define DATA_DEP_H_CHOPPER 

#include <llvm/Pass.h>
#include <llvm/Function.h>

using namespace llvm;

namespace chopper {
    class DataDep : public FunctionPass {
    public:
        static char ID;
        DataDep ();

        virtual bool runOnFunction(Function&);

        virtual void getAnalysisUsage(AnalysisUsage&) const;

        virtual ~DataDep ();
    
    private:
    };
} /* chopper */

#endif
