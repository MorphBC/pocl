// Header for WorkitemLoops function pass.
// 
// Copyright (c) 2012 Pekka Jääskeläinen / TUT
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef _POCL_WORKITEM_LOOPS_H
#define _POCL_WORKITEM_LOOPS_H

#include "llvm/ADT/Twine.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Transforms/Utils/ValueMapper.h"
#include <map>
#include <vector>
#include "WorkitemHandler.h"
#include "ParallelRegion.h"

namespace pocl {
  class Workgroup;

  class WorkitemLoops : public pocl::WorkitemHandler {

  public:
    static char ID;

  WorkitemLoops() : pocl::WorkitemHandler(ID) {}

    virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const;
    virtual bool runOnFunction(llvm::Function &F);

  private:

    typedef std::vector<llvm::BasicBlock *> BasicBlockVector;
    typedef std::set<llvm::Instruction* > InstructionIndex;
    typedef std::vector<llvm::Instruction* > InstructionVec;

    InstructionIndex workGroupVariables;

    llvm::DominatorTree *DT;
    llvm::LoopInfo *LI;

    /* The global variables that store the current local id. */
    llvm::Value *localIdZ, *localIdY, *localIdX;

    ParallelRegion::ParallelRegionVector *original_parallel_regions;

    unsigned size_t_width;

    std::map<std::string, llvm::Instruction*> contextArrays;

    virtual bool ProcessFunction(llvm::Function &F);
    void CreateLoopAround(llvm::BasicBlock *entryBB, llvm::BasicBlock *exitBB, llvm::Value *localIdVar, 
                          size_t LocalSizeForDim);

    void FixMultiRegionVariables(ParallelRegion *region);
    void AddContextSaveRestore
        (llvm::Instruction *instruction, 
         const InstructionIndex& instructionsInRegion);

    llvm::Instruction *AddContextSave(llvm::Instruction *instruction, llvm::Instruction *alloca);
    llvm::Instruction *AddContextRestore
        (llvm::Instruction *instruction, llvm::Instruction *alloca, 
         llvm::Instruction *before=NULL);
    llvm::Instruction *GetContextArray(llvm::Instruction *val);
    llvm::Instruction *BreakPHIToAllocas(llvm::PHINode* phi);

    ParallelRegion* RegionOfBlock(llvm::BasicBlock *bb);
  };
}

#endif