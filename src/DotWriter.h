#ifndef DOT_WRITER_RAZORLEAF
#define DOT_WRITER_RAZORLEAF

#include <llvm/Support/raw_ostream.h>

using namespace llvm;
namespace razorleaf {
struct SDGNode;
struct ProcEntryNode;
struct CallSiteNode;
struct InstNode;
/**
 * A dot writer class for sdg
 */
class DotWriter {
public:
  DotWriter (SDGNode*);

  void write();

  virtual ~DotWriter();

private:
  SDGNode *root;
  raw_fd_ostream *pfs;

  void visit(ProcEntryNode *);
  void visit(CallSiteNode *);
  void visit(InstNode *);
};

}

#endif
