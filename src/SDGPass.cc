
#include "SDGPass.h"
#include "SDGNode.h"
#include "DotWriter.h"
#include "pdg/PDGPass.h"
#include "pdg/PDG.h"

#include <llvm/Analysis/PostDominators.h>
#include <llvm/Analysis/CallGraph.h>
#include <llvm/Argument.h>

using namespace llvm;
using namespace razorleaf;
using std::make_pair;

SDGPass::SDGPass():ModulePass(ID), root(0) {}

void SDGPass::buildSDG() {
}

bool SDGPass::runOnModule(Module &m) {
  // currently only one module
  CallGraph &cg = getAnalysis<CallGraph>();
  vector<ProcEntryNode*> procs;
  for (Function &func : m) {
    if (func.isDeclaration()) continue;
    ProcEntryNode *proc = new ProcEntryNode();
    proc->name = func.getName();
    for (Argument &arg : func.getArgumentList()) {
      FormalNode *formal = new FormalNode();
      formal->name = arg.getName();
      proc->formalNodes.push_back(formal);
    }
    procs.push_back(proc);
    functionMap.insert(make_pair(
          &func, proc));
   
    errs() << "END of pdg content " << func.getName() << "\n";
  }

  Function * rootFunc = cg.getRoot()->getFunction();
  root = functionMap.lookup(rootFunc);
  PDGPass &pdgPass = getAnalysis<PDGPass>(*rootFunc);
  PDG * pdg = pdgPass.getPDG();

  // first iteration fill all direct dependence 
  for (PDG::iterator iter = pdg->begin();
      iter != pdg->end(); iter++) {
      Instruction *inst = iter->inst;
    //if(inst) {
      CallInst *callInst;
      if ((callInst = dyn_cast<CallInst>(inst))) {
        CallSiteNode *callSiteNode = new CallSiteNode();
        callSiteNode->name = callInst->getName();
        callSiteNode->inst = inst;
        callSiteNode->entry = functionMap.lookup(
            callInst->getCalledFunction());

        for (unsigned i = 0; 
            i < callInst->getNumArgOperands();
            i++) {
          ActualNode *actualNode = new ActualNode();
          actualNode->name = callInst->getName();
          actualNode->arg = callInst->getArgOperand(i);
          callSiteNode->actualNodes.push_back(actualNode);
        }
        root->deps.push_back(callSiteNode);

      } else {
        InstNode *instNode = new InstNode();
        instNode->name = inst->getName();
        instNode->inst = inst;
        root->deps.push_back(instNode);
      }
    //}
  }

  // second iteration calculate all dependence
  unsigned counter = 0;
  for (PDG::iterator iter = pdg->begin();
      iter != pdg->end(); iter++, counter++) {
    PDG::AdjList &adjList = iter->adjList;
    for (const PDG::AdjVertex &vertex : adjList) {
      root->deps[counter]->deps.push_back(
          root->deps[vertex.id]);
    }
  }

  DotWriter *dotWriter = new DotWriter(root);
  dotWriter->write();
  delete dotWriter;

  return false;
}

bool SDGPass::doFinalization(Module &cg) {
  errs() << ">>--END--<<\n";
  return false;
}

void SDGPass::getAnalysisUsage(AnalysisUsage &au) const {
  au.setPreservesAll();
  au.addRequired<CallGraph>();
  au.addRequired<PDGPass>();
}

char SDGPass::ID = 0;
static RegisterPass<SDGPass> XSDG("sdg",
    "System Dependence Graph Pass", false, true);

