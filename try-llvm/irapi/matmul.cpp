#include <iostream>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/GlobalValue.h"

using namespace llvm;

int main(void) {
  LLVMContext ctx;
  Module mod("matmul", ctx);
  IRBuilder<> builder(ctx);


  // handy types
  Type* int8Ty = Type::getInt8Ty(ctx);
  Type* int32Ty = Type::getInt32Ty(ctx);
  Type* int64Ty = Type::getInt64Ty(ctx);
  Type* doubleTy = Type::getDoubleTy(ctx);
  Type *int8PtrTy = PointerType::get(int8Ty, 0);
  ArrayType* rowTy = ArrayType::get(doubleTy, 2);
  ArrayType* matTy = ArrayType::get(rowTy, 2);;

  ArrayType* ar2chTy = ArrayType::get(int8Ty, 2);
  ArrayType* ar4chTy = ArrayType::get(int8Ty, 4);

  Value* ci32_0 = ConstantInt::get(int32Ty, 0);
  Value* ci32_1 = ConstantInt::get(int32Ty, 1);
  Value* ci32_2 = ConstantInt::get(int32Ty, 2);
  Value* ci64_0 = ConstantInt::get(int64Ty, 0);
  Value* cdbl_0 = ConstantFP::get(doubleTy, 0.0);

  FunctionCallee printfDecl = mod.getOrInsertFunction("printf",
    FunctionType::get(int32Ty, int8PtrTy, true));

  FunctionType* mainFuncTy = FunctionType::get(
    int32Ty, {}, false
  );
  Function* mainFunc = Function::Create(mainFuncTy, GlobalValue::ExternalLinkage, "main", mod);

  BasicBlock* entryBB = BasicBlock::Create(ctx, "l_entry", mainFunc);
  BasicBlock* mm_i_condBB = BasicBlock::Create(ctx, "l_mm_i_cond", mainFunc);
  BasicBlock* mm_i_bodyBB = BasicBlock::Create(ctx, "l_mm_i_body", mainFunc);
  BasicBlock* mm_j_condBB = BasicBlock::Create(ctx, "l_mm_j_cond", mainFunc);
  BasicBlock* mm_j_bodyBB = BasicBlock::Create(ctx, "l_mm_j_body", mainFunc);
  BasicBlock* mm_k_condBB = BasicBlock::Create(ctx, "l_mm_k_cond", mainFunc);
  BasicBlock* mm_k_bodyBB = BasicBlock::Create(ctx, "l_mm_k_body", mainFunc);
  BasicBlock* mm_k_nextBB = BasicBlock::Create(ctx, "l_mm_k_next", mainFunc);
  BasicBlock* mm_j_nextBB = BasicBlock::Create(ctx, "l_mm_j_next", mainFunc);
  BasicBlock* mm_i_nextBB = BasicBlock::Create(ctx, "l_mm_i_next", mainFunc);
  BasicBlock* pr_i_condBB = BasicBlock::Create(ctx, "l_pr_i_cond", mainFunc);
  BasicBlock* pr_i_bodyBB = BasicBlock::Create(ctx, "l_pr_i_body", mainFunc);
  BasicBlock* pr_j_condBB = BasicBlock::Create(ctx, "l_pr_j_cond", mainFunc);
  BasicBlock* pr_j_bodyBB = BasicBlock::Create(ctx, "l_pr_j_body", mainFunc);
  BasicBlock* pr_j_nextBB = BasicBlock::Create(ctx, "l_pr_j_next", mainFunc);
  BasicBlock* pr_i_nextBB = BasicBlock::Create(ctx, "l_pr_i_next", mainFunc);

  Value *iptr, *jptr, *kptr, *mat1_ptr, *mat2_ptr, *matres_ptr;
  Constant* const_mat1_init = ConstantArray::get(matTy,
    {
      ConstantArray::get(rowTy, {ConstantFP::get(doubleTy, 2.0), ConstantFP::get(doubleTy, 1.0)}),
      ConstantArray::get(rowTy, {ConstantFP::get(doubleTy, 1.0), ConstantFP::get(doubleTy, 1.0)}),
    }
  );
  GlobalVariable* const_mat1 = new GlobalVariable(
    mod, matTy, true, GlobalValue::PrivateLinkage, const_mat1_init, "const_mat1");
  Constant* const_mat2_init = ConstantArray::get(matTy,
    {
      ConstantArray::get(rowTy, {ConstantFP::get(doubleTy, 3.0), ConstantFP::get(doubleTy, 2.0)}),
      ConstantArray::get(rowTy, {ConstantFP::get(doubleTy, 2.0), ConstantFP::get(doubleTy, 1.0)}),
    }
  );
  GlobalVariable* const_mat2 = new GlobalVariable(
    mod, matTy, true, GlobalValue::PrivateLinkage, const_mat2_init, "const_mat2");
  Value *fmtitem = builder.CreateGlobalStringPtr(" %f", "fmtitem", 0, &mod);
  Value *fmtnl = builder.CreateGlobalStringPtr("\n", "fmtnl", 0, &mod);

  { // l_entry
    builder.SetInsertPoint(entryBB);
    iptr = builder.CreateAlloca(int32Ty, nullptr, "iptr");
    jptr = builder.CreateAlloca(int32Ty, nullptr, "jptr");
    kptr = builder.CreateAlloca(int32Ty, nullptr, "kptr");
    mat1_ptr = builder.CreateAlloca(matTy, nullptr, "mat1_ptr");
    mat2_ptr = builder.CreateAlloca(matTy, nullptr, "mat2_ptr");
    matres_ptr = builder.CreateAlloca(matTy, nullptr, "matres_ptr");
    
    Value* flat_mat1_ptr = builder.CreateBitCast(mat1_ptr, int8PtrTy, "flat_mat1_ptr");
    builder.CreateMemCpy(flat_mat1_ptr, MaybeAlign(), const_mat1, MaybeAlign(), 32);
    Value* flat_mat2_ptr = builder.CreateBitCast(mat2_ptr, int8PtrTy, "flat_mat2_ptr");
    builder.CreateMemCpy(flat_mat2_ptr, MaybeAlign(), const_mat2, MaybeAlign(), 32);
    builder.CreateStore(ConstantInt::get(int32Ty, 0), iptr);
    builder.CreateBr(mm_i_condBB);
  }
  { // l_mm_i_cond
    builder.SetInsertPoint(mm_i_condBB);
    Value* i = builder.CreateLoad(int32Ty, iptr, "i");
    Value* cc = builder.CreateICmpSLT(i, ConstantInt::get(int32Ty, 2), "cc");
    builder.CreateCondBr(cc, mm_i_bodyBB, mm_i_nextBB);
  }
  { // l_mm_i_body
    builder.SetInsertPoint(mm_i_bodyBB);
    builder.CreateStore(ConstantInt::get(int32Ty, 0), jptr);
    builder.CreateBr(mm_j_condBB);
  }
  { // l_mm_j_cond
    builder.SetInsertPoint(mm_j_condBB);
    Value* j = builder.CreateLoad(int32Ty, jptr, "j");
    Value* cc_1 = builder.CreateICmpSLT(j, ConstantInt::get(int32Ty, 2), "cc_1");
    builder.CreateCondBr(cc_1, mm_j_bodyBB, mm_j_nextBB);
  }
  { // l_mm_j_body
    builder.SetInsertPoint(mm_j_bodyBB);
    Value* i_1 = builder.CreateLoad(int32Ty, iptr, "i_1");
    Value* i_2 = builder.CreateSExt(i_1, int64Ty, "i_2");
    Value* matres_rowptr = builder.CreateInBoundsGEP(matTy, matres_ptr, { ci64_0, i_2}, "matres_rowptr");
    Value* j_1 = builder.CreateLoad(int32Ty, jptr, "j_1");
    Value* j_2 = builder.CreateSExt(j_1, int64Ty, "j_2");
    Value* matres_itemptr = builder.CreateInBoundsGEP(rowTy, matres_rowptr, { ci64_0, j_2}, "matres_itemptr");
    builder.CreateStore(cdbl_0, matres_itemptr);
    builder.CreateStore(ci32_0, kptr);
    builder.CreateBr(mm_k_condBB);
  }
  { // l_mm_k_cond
    builder.SetInsertPoint(mm_k_condBB);
    Value* k = builder.CreateLoad(int32Ty, kptr, "k");
    Value* cc_2 = builder.CreateICmpSLT(k, ci32_2, "cc_2");
    builder.CreateCondBr(cc_2, mm_k_bodyBB, mm_k_nextBB);
  }
  { // l_mm_k_body
    builder.SetInsertPoint(mm_k_bodyBB);
    Value* i_3 = builder.CreateLoad(int32Ty, iptr, "i_3");
    Value* i_4 = builder.CreateSExt(i_3, int64Ty, "i_4");
    Value* mat1_rowptr = builder.CreateInBoundsGEP(matTy, mat1_ptr, { ci64_0, i_4}, "mat1_rowptr");
    Value* k_1 = builder.CreateLoad(int32Ty, kptr, "k_1");
    Value* k_2 = builder.CreateSExt(k_1, int64Ty, "k_2");
    Value* mat1_itemptr = builder.CreateInBoundsGEP(rowTy, mat1_rowptr, { ci64_0, k_2}, "mat1_itemptr");
    Value* mat1_item = builder.CreateLoad(doubleTy, mat1_itemptr, "mat1_item");

    Value* k_3 = builder.CreateLoad(int32Ty, kptr, "k_3");
    Value* k_4 = builder.CreateSExt(k_3, int64Ty, "k_4");
    Value* mat2_rowptr = builder.CreateInBoundsGEP(matTy, mat2_ptr, {ci64_0, k_4}, "mat2_rowptr");
    Value* j_3 = builder.CreateLoad(int32Ty, jptr, "j_3");
    Value* j_4 = builder.CreateSExt(j_3, int64Ty, "j_4");
    Value* mat2_itemptr = builder.CreateInBoundsGEP(rowTy, mat2_rowptr, {ci64_0, j_4}, "mat2_itemptr");
    Value* mat2_item = builder.CreateLoad(doubleTy, mat2_itemptr, "mat2_item");

    Value* mulres = builder.CreateFMul(mat1_item, mat2_item, "mulres");
    Value* i_5 = builder.CreateLoad(int32Ty, iptr, "i_5");
    Value* i_6 = builder.CreateSExt(i_5, int64Ty, "i_6");
    Value* matres_rowptr_1 = builder.CreateInBoundsGEP(matTy, matres_ptr, {ci64_0, i_6}, "matres_rowptr_1");
    Value* j_5 = builder.CreateLoad(int32Ty, jptr, "j_5");
    Value* j_6 = builder.CreateSExt(j_5, int64Ty, "j_6");
    Value* matres_itemptr_1 = builder.CreateInBoundsGEP(rowTy, matres_rowptr_1, {ci64_0, j_6}, "matres_itemptr_1");
    Value* matres_item = builder.CreateLoad(doubleTy, matres_itemptr_1, "matres_item");
    Value* sumres = builder.CreateFAdd(matres_item, mulres, "sumres");
    builder.CreateStore(sumres, matres_itemptr_1);

    Value* k_5 = builder.CreateLoad(int32Ty, kptr, "k_5");
    Value* k_6 = builder.CreateAdd(k_5, ci32_1, "k_6");
    builder.CreateStore(k_6, kptr);
    builder.CreateBr(mm_k_condBB);
  }
  { // mm_k_nextBB
    builder.SetInsertPoint(mm_k_nextBB);
    Value* j_7 = builder.CreateLoad(int32Ty, jptr, "j_7");
    Value* j_8 = builder.CreateAdd(j_7, ci32_1, "j_8");
    builder.CreateStore(j_8, jptr);
    builder.CreateBr(mm_j_condBB);
  }
  { // mm_j_next
    builder.SetInsertPoint(mm_j_nextBB);
    Value* i_7 = builder.CreateLoad(int32Ty, iptr, "i_7");
    Value* i_8 = builder.CreateAdd(i_7, ci32_1, "i_8");
    builder.CreateStore(i_8, iptr);
    builder.CreateBr(mm_i_condBB);
  }
  { // mm_i_next
    builder.SetInsertPoint(mm_i_nextBB);
    builder.CreateStore(ci32_0, iptr);
    builder.CreateBr(pr_i_condBB);
  }
  { // pr_i_cond
    builder.SetInsertPoint(pr_i_condBB);
    Value* i_9 = builder.CreateLoad(int32Ty, iptr, "i_9");
    Value* cc_3 = builder.CreateICmpSLT(i_9, ci32_2, "cc_3");
    builder.CreateCondBr(cc_3, pr_i_bodyBB, pr_i_nextBB);
  }
  { // pr_i_body
    builder.SetInsertPoint(pr_i_bodyBB);
    builder.CreateStore(ci32_0, jptr);
    builder.CreateBr(pr_j_condBB);
  }
  { // pr_j_cond
    builder.SetInsertPoint(pr_j_condBB);
    Value* j_9 = builder.CreateLoad(int32Ty, jptr, "j_9");
    Value* cc_4 = builder.CreateICmpSLT(j_9, ci32_2, "cc_4");
    builder.CreateCondBr(cc_4, pr_j_bodyBB, pr_j_nextBB);
  }
  { // pr_j_body
    builder.SetInsertPoint(pr_j_bodyBB);
    Value* i_10 = builder.CreateLoad(int32Ty, iptr, "i_10");
    Value* i_11 = builder.CreateSExt(i_10, int64Ty, "i_11");
    Value* matres_rowptr_2 = builder.CreateInBoundsGEP(matTy, matres_ptr, {ci64_0, i_11}, "matres_rowptr_2");
    Value* j_10 = builder.CreateLoad(int32Ty, jptr, "j_10");
    Value* j_11 = builder.CreateSExt(j_10, int64Ty, "j_11");
    Value* matres_itemptr_2 = builder.CreateInBoundsGEP(rowTy, matres_rowptr_2, {ci64_0, j_11}, "matres_itemptr_2");
    Value* matres_item_1 = builder.CreateLoad(doubleTy, matres_itemptr_2, "matres_item_1");
    #if 0
    // need this if we use CreateGlobalString rather than CreateGlobalStringPtr
    // to create fmtitem
    Value* fmtitem_gep = builder.CreateInBoundsGEP(
      ar4chTy, fmtitem, {ci64_0, ci64_0}, "fmtitem_gep");
    #endif
    builder.CreateCall(printfDecl, {fmtitem, matres_item_1});
    Value* j_12 = builder.CreateLoad(int32Ty, jptr, "j_12");
    Value* j_13 = builder.CreateAdd(j_12, ci32_1, "j_13");
    builder.CreateStore(j_13, jptr);
    builder.CreateBr(pr_j_condBB);
  }
  { // pr_j_next
    builder.SetInsertPoint(pr_j_nextBB);
    builder.CreateCall(printfDecl, fmtnl);
    Value* i_12 = builder.CreateLoad(int32Ty, iptr, "i_12");
    Value* i_13 = builder.CreateAdd(i_12, ci32_1, "i_13");
    builder.CreateStore(i_13, iptr);
    builder.CreateBr(pr_i_condBB);
  }
  { // pr_i_next
    builder.SetInsertPoint(pr_i_nextBB);
    builder.CreateRet(ci32_0);
  }

  // write the mod to a file
  std::error_code EC;
  raw_fd_ostream os("/tmp/gen.ll", EC);
  mod.print(os, nullptr);
  std::cout << "bye" << std::endl;
  return 0;
}
