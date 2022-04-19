#include "llvm/IR/PassManager.h"
#include "llvm/IR/PatternMatch.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace std;
using namespace llvm::PatternMatch;

namespace {
class PropagateIntegerEquality : public PassInfoMixin<PropagateIntegerEquality> {
public:
  bool precedes(Argument *XA, Instruction *XI, Argument *YA, Instruction *YI, DominatorTree &DT) {
      return ((XI && YI && DT.dominates(XI, YI)) || (XA && YA && XA->getArgNo() < YA->getArgNo()) ||
             (XA && YI));
  }

  void visit(BasicBlock &BB, DominatorTree &DT) {
    ICmpInst::Predicate Pred;
    Value *X, *Y;
    BasicBlock *BB_true, *BB_false;
    // match "br (icmp eq iN x, y) BB_true BB_false"
    if(match(BB.getTerminator(), m_Br(m_ICmp(Pred, m_Value(X), m_Value(Y)),
      m_BasicBlock(BB_true), m_BasicBlock(BB_false))) && Pred == ICmpInst::ICMP_EQ && X != Y &&
      X->getType()->isIntegerTy() && Y->getType()->isIntegerTy() && BB_true != BB_false) {
      Argument *XA = dyn_cast<Argument>(X), *YA = dyn_cast<Argument>(Y);
      Instruction *XI = dyn_cast<Instruction>(X), *YI = dyn_cast<Instruction>(Y);
      if((XA || XI) && (YA || YI)) {
        Value *A, *B;
        if (precedes(XA, XI, YA, YI, DT)) { A = Y; B = X; }
        else if (precedes(YA, YI, XA, XI, DT)) { A = X; B = Y; }
        else return;
        BasicBlockEdge BBE = BasicBlockEdge(&BB, BB_true);
        // for all A's dominated by BBE, replace them with B
        for (auto itr = A->use_begin(), end = A->use_end(); itr != end;) {
          Use &U = *itr++;
          if (DT.dominates(BBE, U)) U.set(B);
        }
      }
    }
    for (DomTreeNode *NextNode : *DT.getNode(&BB))
      visit(*NextNode->getBlock(), DT);
  }

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    DominatorTree &DT = FAM.getResult<DominatorTreeAnalysis>(F);
    visit(F.getEntryBlock(), DT);
    return PreservedAnalyses::all();
  }
};
}

extern "C" ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "PropagateIntegerEquality", "v0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
           ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "prop-int-eq") {
            FPM.addPass(PropagateIntegerEquality());
            return true;
          }
          return false;
        }
      );
    }
  };
}
