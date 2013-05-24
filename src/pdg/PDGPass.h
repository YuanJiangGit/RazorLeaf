#ifndef DATA_DEP_H_CHOPPER
#define DATA_DEP_H_CHOPPER 

#include <llvm/Pass.h>
#include <llvm/Function.h>
#include <llvm/Module.h>
#include <llvm/Analysis/MemoryDependenceAnalysis.h>
#include <llvm/Analysis/AliasAnalysis.h>

#include <cstdio>

using namespace llvm;

#define SERIALIZE_MARCO "CHOPPER_JSON"

namespace razorleaf {
    class PDG;
    class CDG;
    class PDGPass : public FunctionPass {
    public:
        static char ID;
        PDGPass ();

        virtual bool doInitialization(Module&);

        virtual bool doFinalization(Module&);

        virtual bool runOnFunction(Function&);

        virtual void getAnalysisUsage(AnalysisUsage&) const;

        virtual ~PDGPass ();

        inline PDG* getPDG() const { return pdg; }

        inline CDG* getCDG() const { return cdg; }
    private:
        PDG *pdg;
        CDG *cdg;
        PDG *buildPDG(Function&, 
                MemoryDependenceAnalysis&,
                AliasAnalysis&);
    };
} /* razorleaf */

#endif

