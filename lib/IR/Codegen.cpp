#include "weasel/IR/Codegen.h"

weasel::WeaselCodegen::WeaselCodegen(llvm::LLVMContext *context, const std::string &moduleName)
{
    _context = context;
    _module = new llvm::Module(moduleName, *_context);
    _mdBuilder = new llvm::MDBuilder(*_context);
    _builder = new llvm::IRBuilder<>(*_context);
    _metaData = Metadata(context, *_module);
}
