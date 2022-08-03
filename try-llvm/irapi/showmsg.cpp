#include <iostream>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/APFloat.h"

using namespace llvm;

// reference for creating global string literal using IRBuilder: http://zhangyushao.site/2019/01/29/notes-on-update-llvm-string-pointer.html
int main(void) {
	LLVMContext TheContext;
	IRBuilder<> TheBuilder(TheContext);
	Module TheModule("showmsg module", TheContext);
	Type* intTy = Type::getInt32Ty(TheContext);
	FunctionType *TheFunctionType = FunctionType::get(intTy, {}, false);	
	Function* TheFunction = Function::Create(TheFunctionType, Function::ExternalLinkage, "main", TheModule);
	BasicBlock* EntryBB = BasicBlock::Create(TheContext, "entry", TheFunction);
	TheBuilder.SetInsertPoint(EntryBB);
	Constant* msg = TheBuilder.CreateGlobalStringPtr("LLVM is COOL!\n", "msg");
	FunctionCallee printfFunc = TheModule.getOrInsertFunction("printf",
		FunctionType::get(IntegerType::getInt8PtrTy(TheContext),
			PointerType::get(Type::getInt8Ty(TheContext), 0), true));
	TheBuilder.CreateCall(printfFunc, msg);
	Value* int0 = ConstantInt::get(TheContext, APInt(32, 0, true));
	TheBuilder.CreateRet(int0);
	TheModule.print(errs(), nullptr);
	std::cout << std::endl;
	std::cout << "Bye" << std::endl;
	return 0;
}
