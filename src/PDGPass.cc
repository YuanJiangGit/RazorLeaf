
#include "PDGPass.h"

#include <llvm/Instruction.h>
#include <llvm/BasicBlock.h>
#include <llvm/Analysis/MemoryDependenceAnalysis.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/PostDominators.h>

#include "CtrlDep.h"
#include "GraphWriter.h"
/*
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
*/
#include <string>
#include <vector>
#include <utility>

using namespace chopper;
using std::string;
using std::vector;

char PDGPass::ID = 0;

PDGPass::PDGPass() : FunctionPass(PDGPass::ID)
{
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
    GraphWriter::writeCDG(cdg, cdFilename);
    
    MemoryDependenceAnalysis &mda = 
        getAnalysis<MemoryDependenceAnalysis>();
    AliasAnalysis &aa =
        getAnalysis<AliasAnalysis>();
    /*
    LoopInfo &li =
        getAnalysis<LoopInfo>(); 
    string filename = "dd." + f.getName().str() + ".dot";
    string errorInfo;
    raw_fd_ostream fs(filename.c_str(), errorInfo);

    if (errorInfo.size() > 0) {
        errs() << "error when opening file \n";
        return false;
    }

    InstMap instMap;

    vector<Instruction*> insts;

    //analysis loops

    unsigned int instCounter = 0;
    unsigned int bbCounter = 0;

    fs << "digraph g{\n";
    fs << "label = \"Data dependence in function '" 
        << f.getName() << "\";\n";

    for (BasicBlock &bb : f) {
        fs << "subgraph cluster" << bbCounter 
            << " {\n label = \"" << bb.getName() << "\";\n";
        fs << "color=blue;\n";
        for (Instruction &inst : bb) {
            fs << "inst" << instCounter << "[label=\"" 
                << inst << "\"];\n";
            InstInfo instInfo = { instCounter };
            instMap.insert(std::make_pair(&inst,
                        instInfo));
            instCounter ++;
        }
        fs << "}\n";
        bbCounter ++;
    }

    for (std::pair<Instruction*, InstInfo> &info : instMap) {
        Instruction *inst = info.first;
        for (Instruction::use_iterator iter = inst->use_begin();
                iter != inst->use_end(); iter++) {
            Instruction *use = dyn_cast<Instruction>(*iter);
            if (use) {
                fs << "inst" << info.second.id << " -> inst"
                    << instMap[use].id << ";\n";
            }
        }

        //memory dependence
        if (inst->mayReadOrWriteMemory()) {
            MemDepResult mdaResult = mda.getDependency(inst);
            Instruction *depInst = mdaResult.getInst();
            //MemDepResult mdaResult = MemDepResult::getDef(inst);
            //depInst = mdaResult.getInst();


            errs() << *inst << " \n depends on \n"
                << *depInst  << "\n";
            if (mdaResult.isClobber()) {
                errs() << "Clobber.\n";
                fs << "inst" << instMap[depInst].id 
                    << " -> inst" << info.second.id 
                    << "[color=\"green\"];\n";
            } else if (mdaResult.isDef()) {
                errs() << "Def.\n";
                fs << "inst" << instMap[depInst].id 
                    << " -> inst" << info.second.id 
                    << "[color=\"coral\"];\n";
            } else if (mdaResult.isNonLocal()) {
                SmallVector<NonLocalDepResult, 400> nldResults;
                if (inst->mayReadFromMemory()) {
                    mda.getNonLocalPointerDependency(
                        aa.getLocation(dyn_cast<LoadInst>(inst)),
                        true,
                        inst->getParent(),
                        nldResults);
                } else {
                    mda.getNonLocalPointerDependency(
                        aa.getLocation(dyn_cast<StoreInst>(inst)),
                        false,
                        inst->getParent(),
                        nldResults);
                }
                for (NonLocalDepResult &nldResult : nldResults) {
                    Instruction *depInst = 
                        nldResult.getResult().getInst();
                    if (nldResult.getResult().isDef()) {
                fs << "inst" << instMap[depInst].id 
                    << " -> inst" << info.second.id 
                    << "[color=\"red\"];\n";
                    }
                }
            }
            errs() << "-------------- \n";

        }
    }

    fs << "}\n";

    fs.close();
    */
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


