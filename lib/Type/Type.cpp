#include "weasel/IR/Context.h"
#include "weasel/Type/Type.h"

llvm::Type *weasel::Type::codegen(weasel::Context *context)
{
    return context->codegen(this);
}
