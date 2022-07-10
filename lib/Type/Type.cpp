#include <iostream>
#include <llvm/IR/Type.h>
#include "weasel/Type/Type.h"

llvm::Type *weasel::Type::codegen(weasel::Context *context)
{
    context->codegen(this);
}
