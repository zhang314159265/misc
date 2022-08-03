#include <iostream>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

int main(void) {
  LLVMContext ctx;
  IRBuilder<> builder(ctx);
  Module mod("SimpSum", ctx);
  FunctionType *funcType = FunctionType::get(
    Type::getInt32Ty(ctx),
    {},
    false
  );
  Function* func = Function::Create(funcType, Function::ExternalLinkage, "main", mod);
  BasicBlock* entryBB = BasicBlock::Create(ctx, "l_entry", func);
  BasicBlock* cmpBB = BasicBlock::Create(ctx, "l_cmp", func);
  BasicBlock* bodyBB = BasicBlock::Create(ctx, "l_body", func);
  BasicBlock* nextBB = BasicBlock::Create(ctx, "l_next", func);

  Value* iptr, *sumptr;
  {
    // create entryBB
    builder.SetInsertPoint(entryBB);
    iptr = builder.CreateAlloca(Type::getInt32Ty(ctx), nullptr, "iptr"); // instr is a kind of value
    sumptr = builder.CreateAlloca(Type::getInt32Ty(ctx), nullptr, "sumptr");
    builder.CreateStore(ConstantInt::get(ctx, APInt(32, 0, true)), iptr);
    builder.CreateStore(ConstantInt::get(ctx, APInt(32, 0, true)), sumptr);
    builder.CreateBr(cmpBB);
  }
  {
    // create cmpBB
    builder.SetInsertPoint(cmpBB);
    Value* i = builder.CreateLoad(Type::getInt32Ty(ctx), iptr, "i");
    Value* cmpres = builder.CreateICmpSLE(i, ConstantInt::get(ctx, APInt(32, 100, true)), "cmpres");
    builder.CreateCondBr(cmpres, bodyBB, nextBB);
  }
  {
    // create bodyBB
    builder.SetInsertPoint(bodyBB);
    Value* i = builder.CreateLoad(Type::getInt32Ty(ctx), iptr, "i_2");
    Value* sum = builder.CreateLoad(Type::getInt32Ty(ctx), sumptr, "sum");
    sum = builder.CreateAdd(i, sum, "sum_2");
    builder.CreateStore(sum, sumptr);

    // increment
    i = builder.CreateLoad(Type::getInt32Ty(ctx), iptr, "i_3");
    i = builder.CreateAdd(i, ConstantInt::get(ctx, APInt(32, 1, true)), "i_4");
    builder.CreateStore(i, iptr);
    builder.CreateBr(cmpBB);
  }
  {
    // create nextBB
    builder.SetInsertPoint(nextBB);
    Value* sum = builder.CreateLoad(Type::getInt32Ty(ctx), sumptr, "sum_final");
    FunctionCallee printfCallee = mod.getOrInsertFunction(
      "printf",
      FunctionType::get(
        Type::getInt32Ty(ctx),
        PointerType::get(Type::getInt8Ty(ctx), 0),
        true));
 
    Value* fmtstr = builder.CreateGlobalStringPtr("Sum is %d\n", "fmtstr");
    builder.CreateCall(
      printfCallee,
      {fmtstr, sum}
    );
    builder.CreateRet(ConstantInt::get(ctx, APInt(32, 0, true)));
  }
  std::error_code EC;
  raw_fd_stream stream("/tmp/gen.ll", EC);
  mod.print(stream, nullptr);
  std::cout << "bye" << std::endl;
}
