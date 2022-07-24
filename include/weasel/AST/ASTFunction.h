#pragma once

#include "weasel/Lexer/Token.h"
#include "weasel/AST/ASTStatement.h"

namespace llvm
{
    class Function;
} // namespace llvm

// Function
namespace weasel
{
    // Func
    class Function : public ASTDebug
    {
    private:
        std::string _identifier;
        FunctionType *_funcTy;
        StatementExpression *_body;
        bool _isDefine = false;
        bool _isInline = false;

    public:
        Function(std::string identifier, FunctionType *funcTy) : _identifier(identifier), _funcTy(funcTy) {}

        std::string getIdentifier() const { return _identifier; }
        FunctionType *getFunctionType() const { return _funcTy; }
        std::vector<FunctionArgument *> getArgs() const { return _funcTy->getArgs(); }

        StatementExpression *getBody() const { return _body; }
        void setBody(StatementExpression *body) { _body = body; }

        void setIsDefine(bool val) { _isDefine = val; }
        bool isDefine() const { return _isDefine; }

        void setIsInline(bool val) { _isInline = val; }
        bool isInline() const { return _isInline; }

    public:
        llvm::Function *codegen(Context *c);

    public:
        void debug(int shift);
    };

} // namespace weasel
