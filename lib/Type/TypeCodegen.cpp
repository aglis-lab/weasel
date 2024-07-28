#include <weasel/Type/Type.h>
#include <weasel/IR/Codegen.h>

using namespace weasel;

llvm::Type *StructType::codegen(WeaselCodegen *context)
{
    return context->codegen(this);
}

llvm::Type *Type::codegen(WeaselCodegen *context)
{
    return context->codegen(this);
}
