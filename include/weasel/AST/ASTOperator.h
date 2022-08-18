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

    // Unary Operator Expression
    class UnaryExpression : public Expression
    {
    public:
        enum Operator
        {
            Borrow,
            Dereference,

            // Negation
            Negative,
            Not,
        };

    private:
        Expression *_rhs;
        Operator _op;

    public:
        UnaryExpression(Token token, Operator op, Expression *rhs) : Expression(token), _rhs(rhs), _op(op)
        {
            auto rhsType = rhs->getType();
            if (op == Operator::Borrow)
                rhsType = Type::getPointerType(rhsType);
            else if (op == Operator::Dereference)
                rhsType = rhsType->getContainedType();

            setType(rhsType);
        }

        Expression *getExpression() const { return _rhs; }
        Operator getOperator() const { return _op; }

        ~UnaryExpression()
        {
            delete _rhs;
        }

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };
} // namespace weasel
