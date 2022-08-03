#include <iostream>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

int main(void) {
	LLVMContext ctx;
  IRBuilder<> builder(ctx);
  Module mod("ifelse", ctx);
  FunctionType *funcType = FunctionType::get(
    Type::getInt32Ty(ctx),
    {},
    false
  );
  Function* func = Function::Create(
    funcType,
    Function::ExternalLinkage,
    "main",
    mod);

  BasicBlock* entryBB = BasicBlock::Create(ctx, "l_entry", func);
  BasicBlock* trueBB = BasicBlock::Create(ctx, "l_true", func);
  BasicBlock* falseBB = BasicBlock::Create(ctx, "l_false", func);
  BasicBlock* mergeBB = BasicBlock::Create(ctx, "l_merge", func);

  Value* a_ptr, *b_ptr, *c_ptr, *d_ptr;
  { // create entryBB
    builder.SetInsertPoint(entryBB);
    a_ptr = builder.CreateAlloca(Type::getInt32Ty(ctx), nullptr, "a_ptr");
    b_ptr = builder.CreateAlloca(Type::getInt32Ty(ctx), nullptr, "b_ptr");
    c_ptr = builder.CreateAlloca(Type::getInt32Ty(ctx), nullptr, "c_ptr");
    d_ptr = builder.CreateAlloca(Type::getInt32Ty(ctx), nullptr, "d_ptr");

    builder.CreateStore(
      ConstantInt::get(ctx, APInt(32, 1, true)), a_ptr);
    builder.CreateStore(
      ConstantInt::get(ctx, APInt(32, 2, true)), b_ptr);
    builder.CreateStore(
      ConstantInt::get(ctx, APInt(32, 3, true)), c_ptr);

    Value* a = builder.CreateLoad(Type::getInt32Ty(ctx), a_ptr, "a");
    Value* cmp_res = builder.CreateICmpSGT(
      a,
      ConstantInt::get(ctx, APInt(32, 0, true)), "cmp_res");

    builder.CreateCondBr(cmp_res, trueBB, falseBB);
  }
  { // create trueBB
    builder.SetInsertPoint(trueBB);
    Value* b = builder.CreateLoad(Type::getInt32Ty(ctx), b_ptr, "b");
    Value* c = builder.CreateLoad(Type::getInt32Ty(ctx), c_ptr, "c");
    Value* sum = builder.CreateAdd(b, c, "sum");
    builder.CreateStore(sum, d_ptr);
    builder.CreateBr(mergeBB);
  }
  { // create falseBB
    builder.SetInsertPoint(falseBB);
    Value* b = builder.CreateLoad(Type::getInt32Ty(ctx), b_ptr, "b_1");
    Value* c = builder.CreateLoad(Type::getInt32Ty(ctx), c_ptr, "c_1");
    Value* dif = builder.CreateSub(b, c, "dif");
    builder.CreateStore(dif, d_ptr);
    builder.CreateBr(mergeBB);
  }
  { // create mergeBB
    builder.SetInsertPoint(mergeBB);
    Value* d = builder.CreateLoad(Type::getInt32Ty(ctx), d_ptr, "d");
    FunctionCallee printfCallee = mod.getOrInsertFunction(
      "printf",
      FunctionType::get(
        Type::getInt32Ty(ctx),
        PointerType::get(Type::getInt8Ty(ctx), 0),
        true));
 
    Value* fmtstr = builder.CreateGlobalStringPtr("d is %d\n", "fmtstr");
    builder.CreateCall(
      printfCallee,
      { fmtstr, d}
    );
    builder.CreateRet(ConstantInt::get(ctx, APInt(32, 0, true)));
  }
  std::error_code EC;
  raw_fd_stream stream("/tmp/gen.ll", EC);
  mod.print(stream, nullptr);
	std::cout << "bye" << std::endl;
	return 0;
}
