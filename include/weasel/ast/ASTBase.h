#pragma once

#include <llvm/IR/Value.h>
#include "weasel/Lexer/Token.h"

// Expression Base Type
namespace weasel
{
    // Expression
    class Expression
    {
    protected:
        Token _token; // Token each expression

    public:
        Expression() = default;
        explicit Expression(Token token) : _token(token) {}

        Token getToken() const { return _token; }

        virtual llvm::Value *codegen(Context *context) = 0;
    };

    // Literal Expression
    class LiteralExpression : public Expression
    {
    protected:
        unsigned _width; // width in bit
        unsigned _size;  // size or length of array

    public:
        LiteralExpression(Token token, unsigned width, unsigned size = 1) : Expression(token), _width(width), _size(size) {}
    };

} // namespace weasel
