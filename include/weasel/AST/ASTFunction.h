#pragma once

#include "weasel/Lexer/Token.h"
#include "weasel/AST/ASTStatement.h"

// Function
namespace weasel
{
    // Function
    class Function : public GlobalObject
    {
    private:
        StatementExpression *_body;
        bool _isDefine = false;
        bool _isInline = false;

    public:
        Function(std::string identifier, Type *type) : GlobalObject(Token(), identifier, type) {}

        StatementExpression *getBody() const { return _body; }
        void setBody(StatementExpression *body) { _body = body; }

        void setIsDefine(bool val) { _isDefine = val; }
        bool isDefine() const { return _isDefine; }

        void setIsInline(bool val) { _isInline = val; }
        bool isInline() const { return _isInline; }

    public:
        llvm::Value *codegen(Context *c);
        void debug(int shift);
    };

} // namespace weasel
