#pragma once

#include <llvm/IR/Value.h>
#include "weasel/Lexer/Token.h"

// Declaration of weasel Class
namespace weasel
{
    // Analysis Context
    class Context;

    // Literal Type
    enum class LiteralType
    {
        LiteralNil,
        LiteralBool,
        LiteralNumber,
        LiteralChar,
        LiteralString,
    };

    enum class ParallelType
    {
        None,
        ParallelFunction,
        ParallelKernel,
    };
} // namespace weasel

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
        // virtual ~Expression() = 0;
    };

    // Literal Expression
    class LiteralExpression : public Expression
    {
    protected:
        LiteralType _type;
        unsigned _width;
        unsigned _size;

    public:
        LiteralExpression(Token token, LiteralType type, unsigned width, unsigned size = 1) : Expression(token), _type(type), _width(width), _size(size) {}
    };

} // namespace weasel
