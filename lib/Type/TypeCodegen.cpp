#include <weasel/Type/Type.h>
#include <weasel/IR/Codegen.h>

using namespace weasel;

llvm::Type *FunctionType::accept(Codegen *context)
{
    return context->codegen(this);
}

llvm::Type *StructType::accept(Codegen *context)
{
    return context->codegen(this);
}

llvm::Type *Type::accept(Codegen *context)
{
    return context->codegen(this);
}
