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
            iter != cdg->end(); iter++, counter++) {
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
    fs << "digraph g {\n";
    size_t counter = 0;
    for (PDG::iterator iter = pdg->begin();
            iter != pdg->end(); iter++, counter++) {
        PDG::Vertex &vertex = (*iter);
        if (!vertex.inst) { continue; }
        string edgeLabel = "";
        fs << "inst" << counter << "[label=\""
           << *(vertex.inst) << "\"];\n";
        for (const PDG::AdjVertex &adjVertex : vertex.adjList) {
            if (adjVertex.type & PDG::PDG_MEMDEP) {
                edgeLabel = "[label=\"memory\"]";
            }

            fs << "inst" << counter << " -> "
                << "inst" << adjVertex.id << edgeLabel << ";\n";
        }
    }

    fs << "}\n";
    fs.close();

}

