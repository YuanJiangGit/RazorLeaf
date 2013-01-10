#define DEBUG_TYPE "test"

#include <llvm/Pass.h>
#include <llvm/Function.h>
#include <llvm/Support/CFG.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/Analysis/Dominators.h>
#include <stdio.h>
#include <llvm/ValueSymbolTable.h>
#include <llvm/ADT/DenseMap.h>

#include <utility>

using namespace llvm;

namespace 
{
   struct Test : public FunctionPass {

       static char ID;
       Test() : FunctionPass(ID) {}

       struct _BBInfo {
           unsigned int seqNo; //position in the bblist
           BitVector pDom; //post dominators
       };

       unsigned int seqCounter_;

       typedef struct _BBInfo BBInfo;
       typedef std::pair<BasicBlock*, BasicBlock*> CfgEdge;
       typedef std::vector<BasicBlock*> BBList;
       typedef DenseMap<BasicBlock*, BBInfo> BBMap;

       void traverseCfg(BasicBlock &bb, 
               BBList &bbList, BBMap &bbMap) {

           BBInfo info = {seqCounter_, BitVector};
           seqCounter_ += 1;

           bbMap.insert(std::make_pair(&bb, info));
           bbList.push_back(&bb);

           for (succ_iterator SI = succ_begin(&bb),
                   E = succ_end(&bb); SI != E; ++SI) {
               BasicBlock *succ = *SI;
               if (!bbMap.count(succ)) {
                errs() << " -> " << succ->getName() << " .\n";
                traverseCfg(*succ, bbList, bbMap);
               }
           }
       }

       void traversePdt(DomTreeNode *root,
               const BBList &bbList,
               BBMap &bbMap)
       {

       }

       /**
        * 1. traverse the cfg, store all basic block info
        *    to a map. O(N)
        * 2. traverse the pdt, store the post domination info
        *    in the map. O(N)
        * 3. find valid edges and store them to the edge set.
        *    O(N)
        * 4. find the lowset(least) common ancestor of all 
        *    edges in the edge set and thus retrieve control 
        *    dependence info. O(N^2)?
        */
       virtual bool runOnFunction(Function &F) {
           errs() << "In Func : ";
           errs().write_escaped(F.getName()) << "\n";

           seqCounter_ = 0;

           BBList bbList;
           BBMap bbMap;

           /* process cfg */
           BasicBlock& cfgbb = F.getEntryBlock();
           errs() << "entry block id "  << cfgbb.getName() << ".\n";
           traverseCfg(cfgbb, bbList, bbMap);
       
           /* process post dominator tree */
           PostDominatorTree &pdt = 
               getAnalysis<PostDominatorTree>();
           DomTreeNode *root = pdt.getRootNode();
           BasicBlock *bb = root->getBlock();

           /*
           DomTreeNodeBase<BasicBlock>::iterator 
               iter = root->begin();
           for (; iter != root->end(); iter ++) {
               BasicBlock *lbb = (*iter)->getBlock();
            }
            */
           
           return false;
       }

       virtual void getAnalysisUsage(AnalysisUsage &AU) const {
           AU.setPreservesAll();
           AU.addRequired<PostDominatorTree>();
       }
   };
} /*  */

char Test::ID = 0;
static RegisterPass<Test> X("test", "test pass",
        false, true);

