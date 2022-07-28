#include "weasel/AST/ASTBase.h"

namespace weasel
{
    // Struct Value
    class StructExpression : public GlobalObject
    {
    public:
        StructExpression(Token token, const std::string &name, Type *type) : GlobalObject(token, name, type) {}

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };
} // namespace weasel
