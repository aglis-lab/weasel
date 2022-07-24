#pragma once

// Include Standard Library
#include <string>
#include <vector>

#include "weasel/Type/Type.h"
#include "weasel/Lexer/Token.h"
#include "weasel/Type/Type.h"
#include "weasel/Basic/Cast.h"

namespace llvm
{
    class Value;
} // namespace llvm

// Expression Base Type
namespace weasel
{
    // For debugging purpose
    class ASTDebug
    {
    protected:
        int defaultShift = 2;

    protected:
        void printDebug(const std::string &val, int shift);

    public:
        virtual void debug(int shift) = 0;
    };

    // Expression
    class Expression : public ASTDebug
    {
    protected:
        Token _token; // Token each expression
        Type *_type;

    public:
        Expression() = default;
        Expression(Token token) : _token(token) {}
        Expression(Token token, Type *type) : _token(token), _type(type) {}

        inline Token getToken() const { return _token; }
        inline Type *getType() const { return _type; }
        inline void setType(Type *type) { _type = type; }
        inline bool isNoType() const { return _type == nullptr; }

        virtual llvm::Value *codegen(Context *context) = 0;

    public:
        bool isCompoundExpression();
    };

    // Literal Expression
    class LiteralExpression : public Expression
    {
    public:
        LiteralExpression(Token token, Type *type) : Expression(token, type) {}
    };

    // Function Argument
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

    // Function Type
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
} // namespace weasel
