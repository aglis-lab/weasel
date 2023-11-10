//
// Created by zaen on 27/06/21.
//
#include "weasel/Metadata/Metadata.h"

#define WEASEL_FULL_VERSION "weasel version 0.1.0-experimental"

weasel::Metadata::Metadata(llvm::LLVMContext *context)
{
    _context = context;
    _builder = new llvm::MDBuilder(*context);
}

llvm::Metadata *weasel::Metadata::getFlagsMetadata()
{
    // TODO: return getBuilder()->createConstant(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*getContext()), 4));
    return getBuilder()->createConstant(llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(*getContext()), llvm::APInt(32, 4, false)));
}

llvm::Metadata *weasel::Metadata::getVersionMetadata()
{
    return getBuilder()->createString(WEASEL_FULL_VERSION);
}

llvm::MDNode *weasel::Metadata::getCLVersionMetadata()
{
    auto metas = std::vector<llvm::Metadata *>{
        // getBuilder()->createConstant(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*getContext()), 2)),
        // getBuilder()->createConstant(llvm::ConstantInt::get(llvm::Type::getInt32Ty(*getContext()), 0)),

        getBuilder()->createConstant(llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(*getContext()), llvm::APInt(32, 2, false))),
        getBuilder()->createConstant(llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(*getContext()), llvm::APInt(32, 0, false))),
    };
    return llvm::MDNode::get(*getContext(), metas);
}
