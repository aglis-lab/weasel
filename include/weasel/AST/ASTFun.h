#pragma once

#include <string>
#include "weasel/Lexer/Token.h"

namespace llvm
{
    class Type;
    class Function;
} // namespace llvm

// Function PART
namespace weasel
{
    // Func Arg
    class FunctionArgument
    {
    private:
        Token _token;
        std::string _argName;
        Type *_type;

    public:
        FunctionArgument(Token token, std::string argName, Type *type) : _token(token), _argName(argName), _type(type) {}

        Token getToken() const { return _token; }
        Type *getArgumentType() const { return _type; }
        std::string getArgumentName() const { return _argName; }
    };

    // Func Type
    class FunctionType
    {
    private:
        std::vector<FunctionArgument *> _args;
        Type *_retType;
        bool _isVararg;

    public:
        FunctionType(Type *returnType, std::vector<FunctionArgument *> args, bool vararg) : _args(args), _retType(returnType), _isVararg(vararg) {}

        std::vector<FunctionArgument *> getArgs() const { return _args; }
        Type *getReturnType() const { return _retType; }

        bool getIsVararg() const { return _isVararg; }
    };

    // Func
    class Function : public ASTDebug
    {
    private:
        std::string _identifier;
        FunctionType *_funcTy;
        StatementExpression *_body;
        bool _isDefine = false;
        bool _isInline = false;
        // ParallelType _parallelType;

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

        // void setParallelType(ParallelType parallelType) { _parallelType = parallelType; }
        // ParallelType getParallelType() const { return _parallelType; }

    public:
        llvm::Function *codegen(Context *c);

    public:
        void debug(int shift);
    };

} // namespace weasel
