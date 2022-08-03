#include <iostream>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/APFloat.h"

llvm::LLVMContext TheContext;
llvm::IRBuilder<> Builder(TheContext);
std::unique_ptr<llvm::Module> TheModule
  = std::make_unique<llvm::Module>("my toy module", TheContext);
llvm::Type* doubleTy = llvm::Type::getDoubleTy(TheContext);

void jitrun() {
	// TODO try out JIT
}

int main(void) {
	llvm::FunctionType *FT = llvm::FunctionType::get(doubleTy, std::vector<llvm::Type*>(2, doubleTy), false);
	llvm::Function* TheFunction = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "MyOperatorFunc", TheModule.get());

	llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", TheFunction);
	Builder.SetInsertPoint(BB);
	llvm::Value* res = Builder.CreateFAdd(TheFunction->getArg(0), TheFunction->getArg(1));
	Builder.CreateRet(res);

	TheModule->print(llvm::errs(), nullptr);

	jitrun();

	std::cout << std::endl;
	std::cout << "irapi/showmsg" << std::endl;
	return 0;
}
