#pragma once

#include "weasel/AST/ASTBase.h"

// Expression Without Block PART
namespace weasel
{
    // Return Expression
    class ReturnExpression : public Expression
    {
    private:
        Expression *_value;

    public:
        ReturnExpression(Token token, Expression *value) : Expression(token), _value(value) {}

        Expression *getValue() const { return _value; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Return Expression
    class BreakExpression : public Expression
    {
    private:
        Expression *_value;

    public:
        BreakExpression(Token token, Expression *value) : Expression(token), _value(value) {}

        Expression *getValue() const { return _value; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Return Expression
    class ContinueExpression : public Expression
    {
    private:
        Expression *_value;

    public:
        ContinueExpression(Token token, Expression *value) : Expression(token), _value(value) {}

        Expression *getValue() const { return _value; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Call Expression
    class MethodCallExpression : public Expression
    {
        std::string _identifier;
        std::vector<Expression *> _args;

    public:
        MethodCallExpression(Token token, std::string identifier, std::vector<Expression *> args) : Expression(token), _identifier(identifier), _args(args) {}

        std::string getIdentifier() const { return _identifier; }
        std::vector<Expression *> getArguments() const { return _args; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Variable Expression
    class VariableExpression : public Expression
    {
    private:
        std::string _identifier;
        bool _addressOf;

    public:
        VariableExpression(Token token, std::string identifier, Type *type, bool addressOf = false) : Expression(token, type), _identifier(identifier), _addressOf(addressOf) {}

        std::string getIdentifier() const { return _identifier; }
        bool isAddressOf() const { return _addressOf; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    class ArrayExpression : public VariableExpression
    {
    private:
        Expression *_indexExpr;

    public:
        ArrayExpression(Token token, std::string identifier, Expression *indexExpr, bool addressOf = false) : VariableExpression(token, identifier, nullptr, addressOf), _indexExpr(indexExpr) {}

        Expression *getIndex() const { return _indexExpr; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Declaration Expression
    class DeclarationExpression : public Expression
    {
    private:
        std::string _identifier;
        Qualifier _qualifier;
        Expression *_value;

    public:
        DeclarationExpression(Token token, std::string identifier, Qualifier qualifier, Type *type, Expression *value = nullptr) : Expression(token, type), _identifier(identifier), _qualifier(qualifier), _value(value) {}

        inline Qualifier getQualifier() const { return _qualifier; }
        inline std::string getIdentifier() const { return _identifier; }
        inline Expression *getValue() const { return _value; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Binary Operator Expression
    class BinaryOperatorExpression : public Expression
    {
    private:
        Expression *_lhs;
        Expression *_rhs;

    public:
        BinaryOperatorExpression(Token op, Expression *lhs, Expression *rhs) : Expression(op, lhs->getType()), _lhs(lhs), _rhs(rhs)
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
