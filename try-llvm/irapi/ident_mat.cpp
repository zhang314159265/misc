#include <iostream>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

int main(void) {
  LLVMContext ctx;
  IRBuilder<> builder(ctx);
  Module mod("ident_mat", ctx);

  FunctionType* mainFuncType = FunctionType::get(
    Type::getInt32Ty(ctx),
    {},
    false
  );
  Function* func = Function::Create(mainFuncType, Function::ExternalLinkage, "main", mod);

  BasicBlock* entryBB = BasicBlock::Create(ctx, "l_entry", func);
  BasicBlock* clear_outer_condBB = BasicBlock::Create(ctx, "l_clear_outer_cond", func);
  BasicBlock* clear_outer_bodyBB = BasicBlock::Create(ctx, "l_clear_outer_body", func);
  BasicBlock* clear_inner_condBB = BasicBlock::Create(ctx, "l_clear_inner_cond", func);
  BasicBlock* clear_inner_bodyBB = BasicBlock::Create(ctx, "l_clear_inner_body", func);
  BasicBlock* clear_inner_nextBB = BasicBlock::Create(ctx, "l_clear_inner_next", func);
  BasicBlock* clear_outer_exitBB = BasicBlock::Create(ctx, "l_clear_outer_exit", func);
  BasicBlock* set1_condBB = BasicBlock::Create(ctx, "l_set1_cond", func);
  BasicBlock* set1_bodyBB = BasicBlock::Create(ctx, "l_set1_body", func);
  BasicBlock* set1_nextBB = BasicBlock::Create(ctx, "l_set1_next", func);
  BasicBlock* sum_outer_condBB = BasicBlock::Create(ctx, "l_sum_outer_cond", func);
  BasicBlock* sum_outer_bodyBB = BasicBlock::Create(ctx, "l_sum_outer_body", func);
  BasicBlock* sum_inner_condBB = BasicBlock::Create(ctx, "l_sum_inner_cond", func);
  BasicBlock* sum_inner_bodyBB = BasicBlock::Create(ctx, "l_sum_inner_body", func);
  BasicBlock* sum_inner_nextBB = BasicBlock::Create(ctx, "l_sum_inner_next", func);
  BasicBlock* sum_outer_nextBB = BasicBlock::Create(ctx, "l_sum_outer_next", func);

  Value* iptr, *jptr, *sumptr, *matptr;
  Value* zeroi64 = ConstantInt::get(ctx, APInt(64, 0, true));
  Type* rowType = ArrayType::get(Type::getFloatTy(ctx), 10);
  Type* matType = ArrayType::get(
    rowType,
    10
  );
  { // l_entry
    builder.SetInsertPoint(entryBB);
    iptr = builder.CreateAlloca(Type::getInt32Ty(ctx), nullptr, "iptr");
    jptr = builder.CreateAlloca(Type::getInt32Ty(ctx), nullptr, "jptr");
    sumptr = builder.CreateAlloca(Type::getFloatTy(ctx), nullptr, "sumptr");
    matptr = builder.CreateAlloca(matType,
      nullptr,
      "matptr");

    builder.CreateStore(ConstantInt::get(ctx, APInt(32, 0, true)), iptr);
    builder.CreateBr(clear_outer_condBB);
  }
  { // l_clear_outer_cond
    builder.SetInsertPoint(clear_outer_condBB);
    Value* i = builder.CreateLoad(Type::getInt32Ty(ctx), iptr, "i");
    Value* cmpres = builder.CreateICmpSLT(i, ConstantInt::get(ctx, APInt(32, 10, true)), "cmpres");
    builder.CreateCondBr(cmpres, clear_outer_bodyBB, clear_outer_exitBB);
  }
  { // l_clear_outer_body
    builder.SetInsertPoint(clear_outer_bodyBB);
    builder.CreateStore(ConstantInt::get(ctx, APInt(32, 0, true)), jptr);
    builder.CreateBr(clear_inner_condBB);
  }
  { // l_clear_inner_cond
    builder.SetInsertPoint(clear_inner_condBB);
    Value* j = builder.CreateLoad(Type::getInt32Ty(ctx), jptr, "j");
    Value* cmpres_1 = builder.CreateICmpSLT(j, ConstantInt::get(ctx, APInt(32, 10, true)), "cmpres_1");
    builder.CreateCondBr(cmpres_1, clear_inner_bodyBB, clear_inner_nextBB);
  }
  { // l_clear_inner_body
    builder.SetInsertPoint(clear_inner_bodyBB);
    Value* i_1 = builder.CreateLoad(Type::getInt32Ty(ctx), iptr, "i_1");
    Value* i_2 = builder.CreateSExt(i_1, Type::getInt64Ty(ctx), "i_2");
    Value* matrow_ptr = builder.CreateInBoundsGEP(matType, matptr, {zeroi64, i_2}, "matrow_ptr");
    Value* j_1 = builder.CreateLoad(Type::getInt32Ty(ctx), jptr, "j_1");
    Value* j_2 = builder.CreateSExt(j_1, Type::getInt64Ty(ctx), "j_2");
    Value* item_ptr = builder.CreateInBoundsGEP(rowType, matrow_ptr, {zeroi64, j_2}, "item_ptr");

    // NOTE: use 0.0f rather than 0.0. The former will result in float in IR
    // while the latter results in double.
    builder.CreateStore(ConstantFP::get(ctx, APFloat(0.0f)), item_ptr);

    Value* j_3 = builder.CreateLoad(Type::getInt32Ty(ctx), jptr, "j_3");
    Value* j_4 = builder.CreateAdd(j_3, ConstantInt::get(ctx, APInt(32, 1, true)), "j_4");
    builder.CreateStore(j_4, jptr);
    builder.CreateBr(clear_inner_condBB);
  }
  { // l_clear_inner_next
    builder.SetInsertPoint(clear_inner_nextBB);
    Value* i_3 = builder.CreateLoad(Type::getInt32Ty(ctx), iptr, "i_3");
    Value* i_4 = builder.CreateAdd(i_3, ConstantInt::get(ctx, APInt(32, 1, true)), "i_4");
    builder.CreateStore(i_4, iptr);
    builder.CreateBr(clear_outer_condBB);
  }
  {
    // l_clear_outer_exit
    builder.SetInsertPoint(clear_outer_exitBB);
    builder.CreateStore(ConstantInt::get(ctx, APInt(32, 0, true)), iptr);
    builder.CreateBr(set1_condBB);
  }
  {
    // l_set1_cond
    builder.SetInsertPoint(set1_condBB);
    Value* i_5 = builder.CreateLoad(Type::getInt32Ty(ctx), iptr);
    Value* cmpres_2 = builder.CreateICmpSLT(i_5, ConstantInt::get(ctx, APInt(32, 10, true)), "cmpres_2");
    builder.CreateCondBr(cmpres_2, set1_bodyBB, set1_nextBB);
  }
  {
    // l_set1_body
    builder.SetInsertPoint(set1_bodyBB);
    Value* i_6 = builder.CreateLoad(Type::getInt32Ty(ctx), iptr, "i_6");
    Value* i_7 = builder.CreateSExt(i_6, Type::getInt64Ty(ctx), "i_7");
    Value* matrow_ptr_1 = builder.CreateInBoundsGEP(matType, matptr, {zeroi64, i_7}, "matrow_ptr_1");

    Value* i_8 = builder.CreateLoad(Type::getInt32Ty(ctx), iptr, "i_8");
    Value* i_9 = builder.CreateSExt(i_8, Type::getInt64Ty(ctx), "i_9");
    Value* item_ptr_1 = builder.CreateInBoundsGEP(rowType, matrow_ptr_1, {zeroi64, i_9}, "item_ptr_1");
    builder.CreateStore(ConstantFP::get(ctx, APFloat(1.0f)), item_ptr_1);

    Value* i_10 = builder.CreateLoad(Type::getInt32Ty(ctx), iptr, "i_10");
    Value* i_11 = builder.CreateAdd(i_10, ConstantInt::get(ctx, APInt(32, 1, true)), "i_11");
    builder.CreateStore(i_11, iptr);
    builder.CreateBr(set1_condBB);
  }
  { // l_set1_next
    builder.SetInsertPoint(set1_nextBB);
    builder.CreateStore(ConstantFP::get(ctx, APFloat(0.0f)), sumptr);
    builder.CreateStore(ConstantInt::get(ctx, APInt(32, 0, true)), iptr);
    builder.CreateBr(sum_outer_condBB);
  }
  { // l_sum_outer_cond
    builder.SetInsertPoint(sum_outer_condBB);
    Value* i_12 = builder.CreateLoad(Type::getInt32Ty(ctx), iptr);
    Value* cmpres_3 = builder.CreateICmpSLT(i_12, ConstantInt::get(ctx, APInt(32, 10, true)), "cmpres_3");
    builder.CreateCondBr(cmpres_3, sum_outer_bodyBB, sum_outer_nextBB);
  }
  { // l_sum_outer_body
    builder.SetInsertPoint(sum_outer_bodyBB);
    builder.CreateStore(ConstantInt::get(ctx, APInt(32, 0, true)), jptr);
    builder.CreateBr(sum_inner_condBB);
  }
  { // l_sum_inner_cond
    builder.SetInsertPoint(sum_inner_condBB);
    Value* j_5 = builder.CreateLoad(Type::getInt32Ty(ctx), jptr);
    Value* cmpres_4 = builder.CreateICmpSLT(j_5, ConstantInt::get(ctx, APInt(32, 10, true)), "cmpres_4");
    builder.CreateCondBr(cmpres_4, sum_inner_bodyBB, sum_inner_nextBB);
  }
  { // l_sum_inner_body
    builder.SetInsertPoint(sum_inner_bodyBB);
    Value* i_13 = builder.CreateLoad(Type::getInt32Ty(ctx), iptr, "i_13");
    Value* i_14 = builder.CreateSExt(i_13, Type::getInt64Ty(ctx), "i_14");
    Value* matrow_ptr_2 = builder.CreateInBoundsGEP(matType, matptr, {zeroi64, i_14}, "matrow_ptr_2");

    Value* j_6 = builder.CreateLoad(Type::getInt32Ty(ctx), jptr, "j_6");
    Value* j_7 = builder.CreateSExt(j_6, Type::getInt64Ty(ctx), "j_7");
    Value* item_ptr_2 = builder.CreateInBoundsGEP(rowType, matrow_ptr_2, {zeroi64, j_7}, "item_ptr_2");
    Value* item = builder.CreateLoad(Type::getFloatTy(ctx), item_ptr_2, "item");
    Value* sum = builder.CreateLoad(Type::getFloatTy(ctx), sumptr, "sum");
    Value* sum_2 = builder.CreateFAdd(sum, item, "sum_2");
    builder.CreateStore(sum_2, sumptr);

    Value* j_8 = builder.CreateLoad(Type::getInt32Ty(ctx), jptr, "j_8");
    Value* j_9 = builder.CreateAdd(j_8, ConstantInt::get(ctx, APInt(32, 1, true)), "j_9");
    builder.CreateStore(j_9, jptr);
    builder.CreateBr(sum_inner_condBB);
  }
  { // l_sum_inner_next
    builder.SetInsertPoint(sum_inner_nextBB);
    Value* i_15 = builder.CreateLoad(Type::getInt32Ty(ctx), iptr, "i_15");
    Value* i_16 = builder.CreateAdd(i_15, ConstantInt::get(ctx, APInt(32, 1, true)), "i_16");
    builder.CreateStore(i_16, iptr);
    builder.CreateBr(sum_outer_condBB);
  }
  { // l_sum_outer_next
    builder.SetInsertPoint(sum_outer_nextBB);
    Value* sum_3 = builder.CreateLoad(Type::getFloatTy(ctx), sumptr, "sum_3");
    Value* sum_4 = builder.CreateFPExt(sum_3, Type::getDoubleTy(ctx), "sum_4");

    // call printf
    FunctionCallee printfFunc = mod.getOrInsertFunction("printf",
      FunctionType::get(Type::getInt32Ty(ctx), PointerType::get(Type::getInt8Ty(ctx), 0), true));
    Value* msg = builder.CreateGlobalStringPtr("sum is %f\n", "msg");
    builder.CreateCall(printfFunc, {msg, sum_4});
    builder.CreateRet(ConstantInt::get(ctx, APInt(32, 0, true)));
  }

  std::error_code EC;
  raw_fd_stream stream("/tmp/gen.ll", EC);
  mod.print(stream, nullptr);
  std::cout << "bye" << std::endl;
  return 0;
}
