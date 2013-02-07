
#include "DataDep.h"

#include <llvm/Instruction.h>
#include <llvm/BasicBlock.h>
#include <llvm/Analysis/MemoryDependenceAnalysis.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ADT/DenseMap.h>

#include <string>
#include <vector>
#include <utility>

using namespace chopper;
using std::string;
using std::vector;

char DataDep::ID = 0;

DataDep::DataDep() : FunctionPass(DataDep::ID)
{
}

bool
DataDep::runOnFunction(Function &f)
{
    typedef struct {
        unsigned int id;
    } InstInfo;
    typedef DenseMap <
        Instruction *,
        InstInfo
    > InstMap;

    MemoryDependenceAnalysis &mda = 
        getAnalysis<MemoryDependenceAnalysis>();
    string filename = "dd." + f.getName().str() + ".dot";
    string errorInfo;
    raw_fd_ostream fs(filename.c_str(), errorInfo);

    InstMap instMap;

    vector<Instruction*> insts;

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

        /* memery dependence */
        if (inst->mayReadOrWriteMemory()) {
            MemDepResult mdaResult = mda.getDependency(inst);
            Instruction *inst = mdaResult.getInst();
            fs << "inst" << info.second.id << " -> inst"
                << instMap[inst].id << "[color=\"coral\"];\n";
        }
    }

    fs << "}\n";

    fs.close();
    mda.releaseMemory();


    return false;
}

void
DataDep::getAnalysisUsage(AnalysisUsage &au) const
{
    au.addRequired<MemoryDependenceAnalysis>();
}

DataDep::~DataDep()
{
}

static RegisterPass<DataDep> X("data-dep",
        "data dependence analysis", false, true);


