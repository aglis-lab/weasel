#pragma once

#include "weasel/AST/ASTBase.h"

// Expression Without Block PART
namespace weasel
{
    // Binary Operator Expression
    class BinaryExpression : public Expression
    {
    private:
        Expression *_lhs;
        Expression *_rhs;

    public:
        BinaryExpression(Token op, Expression *lhs, Expression *rhs) : Expression(op, lhs->getType()), _lhs(lhs), _rhs(rhs)
        {
            if (op.isComparison())
            {
                setType(Type::getIntegerType(1, false));
            }
        }

        Token getOperator() const { return getToken(); }
        Expression *getLHS() const { return _lhs; }
        Expression *getRHS() const { return _rhs; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Borrow Operator Expression
    class Borrowxpression : public Expression
    {
    private:
        Expression *_rhs;

    public:
        Borrowxpression(Token token, Expression *rhs) : Expression(token, Type::getPointerType(rhs->getType())), _rhs(rhs) {}

        Expression *getExpression() const { return _rhs; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // TODO: Unary Operator Expression
    class UnaryOperatorExpression : public Expression
    {
    private:
        Token _lhs;
        Expression *_rhs;

    public:
        UnaryOperatorExpression(Token lhs, Expression *rhs) : _lhs(lhs), _rhs(rhs) {}

        llvm::Value *codegen(Context *context) override { return nullptr; }
        void debug(int shift) override;
    };

} // namespace weasel
