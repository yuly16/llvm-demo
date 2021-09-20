#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Optional.h"
#include<iostream>
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

using namespace llvm;
using namespace llvm::sys;
static std::unique_ptr<LLVMContext> TheContext;
static std::unique_ptr<Module> TheModule;
static std::unique_ptr<IRBuilder<>> Builder;
AllocaInst * GlobalAlloca;
GlobalVariable * GlobalV;
static std::map<std::string, Value *> NamedValues;

static void InitializeModule() {
	// Open a new context and module.
	TheContext = std::make_unique<LLVMContext>();
	TheModule = std::make_unique<Module>("my cool jit", *TheContext);

	// Create a new builder for the module.
	Builder = std::make_unique<IRBuilder<>>(*TheContext);
	ConstantFP * constFloat1 = ConstantFP::get(*TheContext, APFloat(2.0));
	GlobalV = new GlobalVariable(*TheModule,
			Type::getDoubleTy(*TheContext),
			false,
			GlobalValue::ExternalLinkage,
			constFloat1,
			"global1");

//	GlobalAlloca = Builder->CreateAlloca(Type::getDoubleTy(*TheContext), 0, "globalAlloca");
//	Builder->CreateStore(GlobalV, GlobalAlloca);
}
/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
//static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction,
//                                          const std::string &VarName) {
//  IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
//                 TheFunction->getEntryBlock().begin());
//  return TmpB.CreateAlloca(Type::getInt32Ty(TheContext), 0,
//                           VarName.c_str());
//}
Function * CreateAddFunction(){
	ConstantFP * constFloat1 = ConstantFP::get(*TheContext, APFloat(1.0));
	Function *TheFunction = TheModule->getFunction("add");
	std::vector<Type*> Doubles(0, Type::getDoubleTy(*TheContext));
	if(!TheFunction){
		// function type
		FunctionType *FT =
			FunctionType::get(Type::getVoidTy(*TheContext), false);

		TheFunction =
			Function::Create(FT, Function::ExternalLinkage, "add", TheModule.get());
	}
	BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
	Builder->SetInsertPoint(BB);
	LoadInst *Load = Builder->CreateLoad(GlobalV);
	Value * RetVal = Builder->CreateFAdd(Load, constFloat1, "addop");
	Builder->CreateStore(RetVal, GlobalV);
	Builder->CreateRetVoid();
	verifyFunction(*TheFunction);
	return TheFunction;
}

Function * CreateSubFunction(){
	ConstantFP * constFloat1 = ConstantFP::get(*TheContext, APFloat(1.0));
	Function *TheFunction = TheModule->getFunction("sub");
	std::vector<Type*> Doubles(0, Type::getDoubleTy(*TheContext));
	if(!TheFunction){
		// function type
		FunctionType *FT =
			FunctionType::get(Type::getVoidTy(*TheContext), false);

		TheFunction =
			Function::Create(FT, Function::ExternalLinkage, "sub", TheModule.get());
	}
	BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
	Builder->SetInsertPoint(BB);
	LoadInst *Load = Builder->CreateLoad(GlobalV);
	Value * RetVal = Builder->CreateFSub(Load, constFloat1, "subop");
	Builder->CreateStore(RetVal, GlobalV);
	Builder->CreateRetVoid();
	verifyFunction(*TheFunction);
	return TheFunction;
}
std::vector<Value *> acquirePrintArgs(){
	//the arguments of printf
	auto formatVal = Builder->CreateGlobalStringPtr("the value of global variable is %f\n");
	std::vector<Value *> printfArgs;
	printfArgs.push_back(formatVal);
	printfArgs.push_back(Builder->CreateLoad(GlobalV));
	return printfArgs;
}
Function * CreatePrintFunction(){
	std::vector<Type *> args;
	args.push_back(Type::getInt8PtrTy(*TheContext));
	/*`true` specifies the function as variadic*/
	FunctionType *printfType =
	  FunctionType::get(Builder->getInt32Ty(), args, true);
	Function::Create(printfType, Function::ExternalLinkage, "printf",
				   TheModule.get());

    std::vector<llvm::Type*> printfFuncArgs;
    printfFuncArgs.push_back(Type::getInt8PtrTy(*TheContext));

    auto printfFuncType = llvm::FunctionType::get(
    	llvm::Type::getInt8PtrTy(*TheContext),
        printfFuncArgs,
        true
    );

    auto printfFunc = Function::Create(printfFuncType, GlobalValue::ExternalLinkage, "printf", TheModule.get());
    return printfFunc;
}
Function * CreateMainFunction(){
	Function *TheFunction = TheModule->getFunction("main");

	if(!TheFunction){
		// function type
		FunctionType *FT =
			FunctionType::get(Type::getVoidTy(*TheContext), false);

		TheFunction =
			Function::Create(FT, Function::ExternalLinkage, "main", TheModule.get());
	}
	BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
	Builder->SetInsertPoint(BB);
	Function *ThePrintFunction = TheModule->getFunction("printf");
	Function *TheAddFunction = TheModule->getFunction("add");
	Function *TheSubFunction = TheModule->getFunction("sub");
//

	//the arguments of add/sub
	std::vector<Value *> noArgs;



	Builder->CreateCall(ThePrintFunction, acquirePrintArgs());
	Builder->CreateCall(TheAddFunction, noArgs);
	Builder->CreateCall(ThePrintFunction, acquirePrintArgs());
	Builder->CreateCall(TheAddFunction, noArgs);
	Builder->CreateCall(ThePrintFunction, acquirePrintArgs());
	Builder->CreateCall(TheSubFunction, noArgs);
	Builder->CreateCall(ThePrintFunction, acquirePrintArgs());
	Builder->CreateCall(TheSubFunction, noArgs);
	Builder->CreateCall(ThePrintFunction, acquirePrintArgs());
	Builder->CreateRetVoid();
	verifyFunction(*TheFunction);
	return TheFunction;
}

void GenerateOutput(){
	// Initialize the target registry etc.
	InitializeAllTargetInfos();
	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmParsers();
	InitializeAllAsmPrinters();

	auto TargetTriple = sys::getDefaultTargetTriple();
	TheModule->setTargetTriple(TargetTriple);

	std::string Error;
	auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

	// Print an error and exit if we couldn't find the requested target.
	// This generally occurs if we've forgotten to initialise the
	// TargetRegistry or we have a bogus target triple.
	if (!Target) {
		errs() << Error;
		exit (EXIT_FAILURE);
	}

	auto CPU = "generic";
	auto Features = "";

	TargetOptions opt;
	auto RM = Optional<Reloc::Model>();
	auto TheTargetMachine =
	  Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

	TheModule->setDataLayout(TheTargetMachine->createDataLayout());

	auto Filename = "output.o";
	std::error_code EC;
	raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);

	if (EC) {
		errs() << "Could not open file: " << EC.message();
		exit (EXIT_FAILURE);
	}

	legacy::PassManager pass;
	auto FileType = CGFT_ObjectFile;

	if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
		errs() << "TheTargetMachine can't emit a file of this type";
		exit (EXIT_FAILURE);
	}

	pass.run(*TheModule);
	dest.flush();

	outs() << "Wrote " << Filename << "\n";
}
int main(){
	// Initialize the Module
	InitializeModule();
//	InitializeAllTargetInfos();
//	InitializeAllTargets();
//	InitializeAllTargetMCs();
//	InitializeAllAsmParsers();
//	InitializeAllAsmPrinters();
	// create a global variable

	Function * TheSubFunction = CreateSubFunction();
	Function * TheAddFunction = CreateAddFunction();
	Function * ThePrintFunction = CreatePrintFunction();
	Function * TheMainFunction = CreateMainFunction();

	TheModule->print(outs(), nullptr);
//	GenerateOutput();


	return 0;
}
