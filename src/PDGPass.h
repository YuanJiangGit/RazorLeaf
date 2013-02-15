#ifndef DATA_DEP_H_CHOPPER
#define DATA_DEP_H_CHOPPER 

#include <llvm/Pass.h>
#include <llvm/Function.h>

using namespace llvm;

namespace chopper {
    class PDGPass : public FunctionPass {
    public:
        static char ID;
        PDGPass ();

        virtual bool runOnFunction(Function&);

        virtual void getAnalysisUsage(AnalysisUsage&) const;

        virtual ~PDGPass ();
    
    private:
    };
} /* chopper */

#endif
