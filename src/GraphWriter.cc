#include "GraphWriter.h"

#include <llvm/Support/raw_ostream.h>
#include <string>
#include "PDG.h"
#include "CtrlDep.h"

using namespace llvm;
using namespace chopper;

using std::string;

void
GraphWriter::writeCDG(CDG* cdg, string& filename)
{
    string errorInfo;
    raw_fd_ostream fs(filename.c_str(), errorInfo);

    if (errorInfo.length() > 0) {
        //TODO exception
    }

    fs << "digraph g {\n";

    size_t counter = 0;
    for (CDG::iterator iter = cdg->begin();
            iter != cdg->end(); iter++) {
        CDG::Vertex &vertex = (*iter);
        string bbName;
        if (vertex.bb) {
            bbName = vertex.bb->getName().str();
        } else {
            bbName = "START";
        }
        fs << "bb" << counter << "[label=\""
            << bbName << "\"];\n";
        for (unsigned int id : vertex.adjList) {
            fs << "bb" << counter << " -> "
                << "bb" << id << ";\n";
        }
        counter ++;
    }

    fs << "}\n";

    fs.close();

}

void
GraphWriter::writePDG(PDG* pdg, string& filename)
{
    string errorInfo;
    raw_fd_ostream fs(filename.c_str(), errorInfo);

    if (errorInfo.length() > 0) {
        //TODO exception
    }

    fs.close();

}

