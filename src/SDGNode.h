#ifndef SDG_NODE_H_RAZORLEAF
#define SDG_NODE_H_RAZORLEAF 

#include <llvm/ADT/StringRef.h>
#include <llvm/Instruction.h>

#include <vector>

using namespace llvm;
using std::vector;

namespace razorleaf {
  struct SDGNode;
  struct CallSiteNode;
  struct ProcEntryNode;
  struct ParameterNode;
  struct FormalNode;
  struct ActualNode;
  struct InstNode;

  struct SDGNode {
    // make it polymorphic
    virtual void f() {}
    //this arrow to
    vector<SDGNode*> deps;
    StringRef name;
  };

  struct ParameterNode : public SDGNode {
    bool isIn;
  };

  struct ActualNode : public ParameterNode {
    Value *arg;
  };

  struct FormalNode : public ParameterNode {
  };

  struct InstNode : public SDGNode {
    Instruction *inst;
  };

  struct CallSiteNode : public InstNode {
    vector<ActualNode*> actualNodes;
    ProcEntryNode *entry;
  };

  struct ProcEntryNode : public SDGNode {
    vector<FormalNode*> formalNodes;
  };

} /* razorleaf */

#endif
