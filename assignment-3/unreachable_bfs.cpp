#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include<string>
using namespace llvm;

namespace {
class MyUnreachablePass : public PassInfoMixin<MyUnreachablePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    std::set<BasicBlock *> check;
    std::queue<BasicBlock *> queue;
    BasicBlock &EB = F.getEntryBlock();

    check.insert(&EB);
    queue.push(&EB);

    while (!queue.empty()) {
      BasicBlock *BB = queue.front();
      queue.pop();
      
      unsigned int numS = BB->getTerminator()->getNumSuccessors();
      for (unsigned int i = 0; i < numS; i++) {
        BasicBlock *S = BB->getTerminator()->getSuccessor(i);

        if (check.find(S) == check.end()) {
          check.insert(S);
          queue.push(S);
        }
      }
    }

    std::vector<StringRef> UR;

    for (BasicBlock &BB : F)
      if (check.find(&BB) == check.end())
        UR.emplace_back(BB.getName());
    
    std::sort(UR.begin(), UR.end());

    for (StringRef &SR : UR)
      outs() << SR << "\n";
    return PreservedAnalyses::all();
  }
};
}

extern "C" ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "MyUnreachablePass", "v0.1",
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM,
           ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "my-unreachable") {
            FPM.addPass(MyUnreachablePass());
            return true;
          }
          return false;
        }
      );
    }
  };
}
