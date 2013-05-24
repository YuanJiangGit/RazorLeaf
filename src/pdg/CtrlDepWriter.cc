#include "CtrlDepWriter.h"

#include <llvm/Support/raw_ostream.h>
#include <llvm/Instruction.h>

#include <set>

using namespace razorleaf;
using namespace llvm;

using std::set;

CtrlDepWriter::CtrlDepWriter()
{}


CtrlDepWriter::~CtrlDepWriter()
{}

static void
write_basicblock(
        llvm::raw_fd_ostream &fs,
        llvm::BasicBlock *bb,
        size_t bbId) {
    fs << bbId << "[shape=record label=\"{" ;
    for (llvm::Instruction& inst : bb->getInstList()) {
        fs << inst << "\\l ";
    }
    fs << "}\"];\n";
}

void
CtrlDepWriter::write(CtrlDep *cdPass)
{
    std::vector<CtrlDep::CtrlDepInfo> 
        infos = cdPass->cds;


    for (CtrlDep::CtrlDepInfo info :
            infos) {
        CtrlDep::BBMap &bbMap = info.bbMap;
        /*
        CtrlDep::CtrlDepMap info.cdMap;
        */
        std::string filename = "cd." + info.func.str() + ".dot";
        std::string errorInfo;
        llvm::raw_fd_ostream fs(filename.c_str(),
                errorInfo);
        /* reconds the id  */
        set<size_t> idSet;

        fs << "digraph G {\n"
           << "label=\"Control Dependence in function " 
           << info.func << "\";";

        for (std::pair<
                BasicBlock*, 
                std::set<BasicBlock*> 
             > cd : info.cdMap) {
            // parent id
            size_t pid = bbMap[cd.first];

            if (!idSet.count(pid)) {
               write_basicblock(fs, cd.first, pid);
               idSet.insert(pid);
            }

            for (BasicBlock *child : cd.second) {
                size_t cid = bbMap[child];

                if (!idSet.count(cid)) {
                    write_basicblock(fs, child, cid);
                    idSet.insert(cid);
                }

                fs << pid << "->" << cid << ";\n";
            }
        }

        fs << "}\n";
        fs.close();

    }

}

