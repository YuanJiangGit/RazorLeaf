
#include "PDGPass.h"

#include <llvm/Instruction.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instruction.h>
#include <llvm/Instructions.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/PostDominators.h>
#include <cstdlib>

#include "CtrlDep.h"
#include "GraphWriter.h"
#include "PDG.h"
#include "Serializer.h"
/*
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
*/
#include <string>
#include <vector>
#include <utility>
#include <cassert>

using namespace chopper;
using std::string;
using std::vector;

char PDGPass::ID = 0;

PDGPass::PDGPass() : FunctionPass(PDGPass::ID)
{
}

bool
PDGPass::doInitialization(Module &m)
{

    char *jsonFilename;
    if (!(jsonFilename = getenv(SERIALIZE_MARCO))) {
        errs() << "set marco CHOPPER_JSON .\n";
        assert(false);
    }
    if (!(outfile = fopen(jsonFilename, "w+"))) {
        errs() << "fail to open " << jsonFilename << ".\n";
        assert(false);
    }

    fwrite("[\n", sizeof(char), 2, outfile);
    return false;
}

bool
PDGPass::doFinalization(Module &m)
{
    fseek(outfile, -1, SEEK_CUR);
    fwrite("]\n", sizeof(char), 2, outfile);
    fclose(outfile);
    return false;
}

PDG*
PDGPass::buildPDG(Function &f, 
        MemoryDependenceAnalysis &mda, 
        AliasAnalysis &aa)
{
    PDG *pdg = new PDG();

    for (BasicBlock &bb : f) {
        for (Instruction &inst : bb) {
            // find all uses
            for (Instruction::use_iterator iter = inst.use_begin();
                    iter != inst.use_end(); iter++) {
                Instruction *use = dyn_cast<Instruction>(*iter);
                if (use) {
                    pdg->addEdge(&inst, use, 0);
                }
            }

            // memeory dependence
            if (inst.mayReadOrWriteMemory()) {
                MemDepResult mdaResult = mda.getDependency(&inst);
                //Instruction *depInst = mdaResult.getInst();
                //MemDepResult mdaResult = MemDepResult::getDef(inst);

                if (mdaResult.isDef()) {
                    pdg->addEdge(mdaResult.getInst(),
                            &inst, PDG::PDG_MEMDEP);
                } else if (mdaResult.isNonLocal()) {
                    SmallVector<NonLocalDepResult, 400> nldResults;
                    AliasAnalysis::Location loc;

                    LoadInst *LI;
                    StoreInst *SI;
                    VAArgInst *VI;
                    AtomicCmpXchgInst *CXI;
                    AtomicRMWInst *RMWI;
                    if ((LI = dyn_cast<LoadInst>(&inst))) {
                        loc = aa.getLocation(LI);
                    } else if ((SI = dyn_cast<StoreInst>(&inst))) {
                        loc = aa.getLocation(SI);
                    } else if ((VI = dyn_cast<VAArgInst>(&inst))) {
                        loc = aa.getLocation(VI);
                    } else if ((CXI = 
                                dyn_cast<AtomicCmpXchgInst>(&inst))) {
                        loc = aa.getLocation(CXI);
                    } else if ((RMWI = 
                                dyn_cast<AtomicRMWInst>(&inst))) {
                        loc = aa.getLocation(RMWI);
                    } else {
                        continue;
                    }
                        
                    if (inst.mayReadFromMemory()) {
                        mda.getNonLocalPointerDependency(
                            loc, true, inst.getParent(), nldResults);
                    } else {
                        mda.getNonLocalPointerDependency(
                            loc, true, inst.getParent(), nldResults);
                    }
                    for (NonLocalDepResult &nldResult : nldResults) {
                        Instruction *depInst = 
                            nldResult.getResult().getInst();
                    //    if (nldResult.getResult().isDef()) {
                            pdg->addEdge(depInst,&inst,PDG::PDG_MEMDEP);
                     //   }
                    }
                } // end of non local
            }

        }
    }
    return pdg;
}

bool
PDGPass::runOnFunction(Function &f)
{
    typedef struct {
        unsigned int id;
    } InstInfo;
    typedef DenseMap <
        Instruction *,
        InstInfo
    > InstMap;

    errs() << "in function " << f.getName() << " .\n";

    PostDominatorTree &pdt =
        getAnalysis<PostDominatorTree>();
    CtrlDep cd(&f, &pdt);
    CDG *cdg = cd.getCDG();
    //pdt.releaseMemory();
    string cdFilename = "cd." + f.getName().str() + ".dot";
    //GraphWriter::writeCDG(cdg, cdFilename);
    
    MemoryDependenceAnalysis &mda = 
        getAnalysis<MemoryDependenceAnalysis>();
    AliasAnalysis &aa =
        getAnalysis<AliasAnalysis>();
    PDG *pdg = buildPDG(f, mda, aa);
    string ddFilename = "dd." + f.getName().str() + ".dot";

    
    //GraphWriter::writePDG(pdg, ddFilename);
    Serializer::SerialInfo info = {
        outfile,
        f.getName().str(),
        pdg, cdg, &f
    };
    Serializer::serialize(info);
    fwrite("\n,", sizeof(char), 2, outfile);
    mda.releaseMemory();

    return false;
}


void
PDGPass::getAnalysisUsage(AnalysisUsage &au) const
{
    //au.addRequired<LoopInfo>();
    au.addRequiredTransitive<AliasAnalysis>();
    au.addRequired<MemoryDependenceAnalysis>();
    au.addRequired<PostDominatorTree>();
}

PDGPass::~PDGPass()
{
}

static RegisterPass<PDGPass> X("pdg",
        "program dependence graph", false, true);


