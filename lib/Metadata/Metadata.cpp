//
// Created by zaen on 27/06/21.
//
#include "weasel/Metadata/Metadata.h"

#define WEASEL_FULL_VERSION "weasel version 0.1.0-experimental"

weasel::Metadata::Metadata(llvm::LLVMContext *context, llvm::Module &module)
{
    _context = context;
    _MDbuilder = new llvm::MDBuilder(*context);
    _DIBuilder = new llvm::DIBuilder(module);
}

llvm::Metadata *weasel::Metadata::getFlagsMetadata()
{
    // TODO: return getBuilder()->createConstant(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*getContext()), 4));
    return getMDBuilder()->createConstant(llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(*getContext()), llvm::APInt(32, 4, false)));
}

llvm::IntrinsicInst *weasel::Metadata::createDbgDeclare(llvm::IRBuilder<> *builder, llvm::Module *module, llvm::Value *value, llvm::Metadata *variableMeta, llvm::Metadata *locationMeta)
{
    // DILocalVariable* iVar = DILocalVariable::Create(GetGlobalContext(), "i", nullptr, nullptr, 0, false, true, 4);
    // llvm::DILocalVariable* iVar = llvm::DILocalVariable::get(GetGlobalContext(), "i", nullptr, nullptr, 0, false, true, 4);
    // llvm::DILocalVariable *iVar = llvm::DILocalVariable::get(*getContext(), llvm::DIVariable::getScope(), "i")

    // auto diLocalVariable = getDIBuilder()->createLocalVariable("i", Type::getInt32Ty(GetGlobalContext()), nullptr);
    auto diLocal = llvm::MetadataAsValue::get(*getContext(), getMDBuilder()->createConstant(llvm::Constant::getIntegerValue(builder->getInt32Ty(), llvm::APInt(32, 0))));
    auto diExpression = getDIBuilder()->createExpression();
    auto valueMeta = llvm::ValueAsMetadata::get(value)->getValue();
    auto args = {valueMeta, llvm::dyn_cast<llvm::Value>(diLocal), llvm::dyn_cast<llvm::Value>(llvm::MetadataAsValue::get(*getContext(), diExpression))};
    auto callee = llvm::dyn_cast<llvm::IntrinsicInst>(builder->CreateIntrinsic(llvm::Intrinsic::dbg_declare, {}, args));
    llvm::MDNode *iAddrMD = llvm::MDNode::get(*getContext(), llvm::ValueAsMetadata::get(value));
    callee->setMetadata("dbg", iAddrMD);

    return callee;
    // Create a DILocalVariable metadata node that describes the variable `i`.
    // llvm::DILocalVariable *iVar = llvm::DILocalVariable::Create(GetGlobalContext(), "i", nullptr, nullptr, 0, false, true, 4);

    // Create a DIExpression metadata node that describes the location of the variable `i` in the source code.
    // DIExpression *iExpr = DIExpression::Create(GetGlobalContext(), DIExpression::Fragment::Constant(0));

    // Declare the local variable `i` to the debugger.
    // CallInst::Create(Intrinsic::getDbgDeclareInst(GetGlobalContext()), {iAddr, iVar, iExpr}, "", nullptr);

    // Create the intrinsic call.
    // std::vector<llvm::Value *> args = {address, variableMeta};
    // auto *dgbDeclare = getBuilder()->CreateCall(
    //     llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::dbg_declare),
    //     args);

    // // Set the debug location.
    // // dbg_declare->

    // // Return the intrinsic call.
    // return llvm::dyn_cast<llvm::IntrinsicInst>(dgbDeclare);
}

llvm::Metadata *weasel::Metadata::getVersionMetadata()
{
    return getMDBuilder()->createString(WEASEL_FULL_VERSION);
}

llvm::MDNode *weasel::Metadata::getCLVersionMetadata()
{
    auto metas = std::vector<llvm::Metadata *>{
        // getBuilder()->createConstant(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*getContext()), 2)),
        // getBuilder()->createConstant(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*getContext()), 0)),

        getMDBuilder()->createConstant(llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(*getContext()), llvm::APInt(32, 2, false))),
        getMDBuilder()->createConstant(llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(*getContext()), llvm::APInt(32, 0, false))),
    };
    return llvm::MDNode::get(*getContext(), metas);
}
