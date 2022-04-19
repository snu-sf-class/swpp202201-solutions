#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
using namespace llvm;

namespace {
class MyUnreachablePass : public PassInfoMixin<MyUnreachablePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    DominatorTree &DT = FAM.getResult<DominatorTreeAnalysis>(F);
    std::vector<StringRef> UR;
    for (BasicBlock &BB : F) if(!DT.isReachableFromEntry(&BB)) UR.emplace_back(BB.getName());
    std::sort(UR.begin(), UR.end());
    for (StringRef &SR : UR) outs() << SR << "\n";
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
