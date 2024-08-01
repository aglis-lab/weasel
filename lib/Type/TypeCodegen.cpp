#include <weasel/Type/Type.h>
#include <weasel/IR/Codegen.h>

using namespace weasel;

llvm::Type *FunctionType::accept(WeaselCodegen *context)
{
    return context->codegen(this);
}

llvm::Type *StructType::accept(WeaselCodegen *context)
{
    return context->codegen(this);
}

llvm::Type *Type::accept(WeaselCodegen *context)
{
    return context->codegen(this);
}
