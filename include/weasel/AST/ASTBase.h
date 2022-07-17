#pragma once

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
} // namespace weasel
