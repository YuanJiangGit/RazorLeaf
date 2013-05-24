#include "DotWriter.h"

#include "SDGNode.h"

#include <string>

using namespace razorleaf;
using namespace llvm;
using std::string;

DotWriter::DotWriter(SDGNode *node):root(node) {
  //TODO specify the filename
  string filename = "sdg.dot";
  string errorInfo;
  pfs = 0;
  if (errorInfo.length() > 0) {
    errs() << "fail to open " << filename << " .\n";
  } else {
    pfs = new raw_fd_ostream(filename.c_str(), errorInfo);
  }
}

void
DotWriter::visit(ProcEntryNode *node) {
  raw_fd_ostream &fs = *pfs;
  fs << node->name << ";\n";
  for(SDGNode* child : node->deps) {
    fs << node->name << " -> " << 
      *((InstNode*)child)->inst << ";\n";
  }
  fs << "\n";
}

void
DotWriter::visit(CallSiteNode *node) {
}

void
DotWriter::visit(InstNode *node) {
}

void
DotWriter::write() {
  raw_fd_ostream &fs = *pfs;
  
  fs << "digraph g { \n";

  visit((ProcEntryNode*)root);

  fs << "} \n";

  fs.close();

}

DotWriter::~DotWriter() {
  if (pfs) delete pfs;
}

