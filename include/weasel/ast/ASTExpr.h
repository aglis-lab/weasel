#pragma once

#include <vector>
#include "weasel/AST/ASTBase.h"

///// Expression /////
// VariableExpression
// LiteralExpression
// ArrayLiteralExpression
// | PathExpression
// OperatorExpression
// | GroupedExpression
// ArrayExpression
// | AwaitExpression
// | IndexExpression
// | TupleExpression
// | TupleIndexingExpression
// | StructExpression
// CallExpression
// | MethodCallExpression
// | FieldExpression
// | ClosureExpression
// | ContinueExpression
// | BreakExpression
// | RangeExpression
// ReturnExpression

/// Operator Expression ///
// UnaryOperatorExpression
// | BorrowExpression
// | DereferenceExpression
// | ErrorPropagationExpression
// | NegationExpression
// | ArithmeticOrLogicalExpression
// | ComparisonExpression
// | LazyBooleanExpression
// | TypeCastExpression
// | AssignmentExpression
// | CompoundAssignmentExpression

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
    };

    // Call Expression
    class CallExpression : public Expression
    {
        std::string _identifier;
        std::vector<Expression *> _args;

    public:
        CallExpression(Token token, std::string identifier, std::vector<Expression *> args) : Expression(token), _identifier(identifier), _args(args) {}

        std::string getIdentifier() const { return _identifier; }
        std::vector<Expression *> getArguments() const { return _args; }

        llvm::Value *codegen(Context *context) override;
    };

    // Variable Expression
    class VariableExpression : public Expression
    {
    private:
        std::string _identifier;
        bool _addressOf;

    public:
        VariableExpression(Token token, std::string identifier, bool addressOf = false) : Expression(token), _identifier(identifier), _addressOf(addressOf) {}

        std::string getIdentifier() const { return _identifier; }
        bool isAddressOf() const { return _addressOf; }

        llvm::Value *codegen(Context *context) override;
    };

    class ArrayExpression : public VariableExpression
    {
    private:
        Expression *_indexExpr;

    public:
        ArrayExpression(Token token, std::string identifier, Expression *indexExpr, bool addressOf = false) : VariableExpression(token, identifier, addressOf), _indexExpr(indexExpr) {}

        Expression *getIndex() const { return _indexExpr; }

        llvm::Value *codegen(Context *context) override;
    };

    // Declaration Expression
    class DeclarationExpression : public Expression
    {
    private:
        std::string _identifier;
        Qualifier _qualifier;
        llvm::Type *_type;
        Expression *_value;

    public:
        DeclarationExpression(Token token, std::string identifier, Qualifier qualifier, llvm::Type *type = nullptr, Expression *value = nullptr) : Expression(token), _identifier(identifier), _qualifier(qualifier), _type(type), _value(value) {}

        std::string getIdentifier() const { return _identifier; }
        Expression *getValue() const { return _value; }
        llvm::Type *getType() const { return _type; }
        Qualifier getQualifier() const { return _qualifier; }

        llvm::Value *codegen(Context *context) override;
    };

    // Number Literal Expression
    class NumberLiteralExpression : public LiteralExpression
    {
    private:
        long long _value; // 64 bit(8 bytes)

    public:
        NumberLiteralExpression(Token token, long long value, unsigned width = 32) : LiteralExpression(token, LiteralType::LiteralNumber, width), _value(value) {}

        long long getValue() const { return _value; }

        llvm::Value *codegen(Context *context) override;
    };

    // Boolean Literal Expression
    class BoolLiteralExpression : public LiteralExpression
    {
    private:
        bool _value;

    public:
        BoolLiteralExpression(Token token, bool value) : LiteralExpression(token, LiteralType::LiteralBool, 1), _value(value) {}

        bool getValue() const { return _value; }

        llvm::Value *codegen(Context *context) override { return nullptr; }
    };

    // String Literal Expression
    class StringLiteralExpression : public LiteralExpression
    {
    private:
        std::string _value;

    public:
        StringLiteralExpression(Token token, const std::string &value) : LiteralExpression(token, LiteralType::LiteralString, 8, value.size()), _value(value) {}

        std::string getValue() const { return _value; }

        llvm::Value *codegen(Context *context) override;
    };

    // Nil Literal Expression
    class NilLiteralExpression : public LiteralExpression
    {
    public:
        explicit NilLiteralExpression(Token token) : LiteralExpression(token, LiteralType::LiteralNil, 64) {}

        llvm::Value *codegen(Context *context) override;
    };

    // Binary Operator Expression
    class BinaryOperatorExpression : public Expression
    {
    private:
        Token _operator;
        Expression *_lhs;
        Expression *_rhs;

    public:
        BinaryOperatorExpression(Token op, Expression *lhs, Expression *rhs) : _operator(op), _lhs(lhs), _rhs(rhs) {}

        Token getOperator() const { return _operator; }
        Expression *getLHS() const { return _lhs; }
        Expression *getRHS() const { return _rhs; }

        llvm::Value *codegen(Context *context) override;
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
    };

    // Array Expression
    class ArrayLiteralExpression : public Expression
    {
    private:
        std::vector<Expression *> _items;

    public:
        ArrayLiteralExpression() = default;
        explicit ArrayLiteralExpression(std::vector<Expression *> items) : _items(items) {}

        void addItem(Expression *item) { _items.push_back(item); }
        std::vector<Expression *> getItems() const { return _items; }

        llvm::Value *codegen(Context *context) override;
    };

} // namespace weasel

//

//

// Expression With Block Function PART
namespace weasel
{
    // Statement Expression
    class StatementExpression : public Expression
    {
    private:
        std::vector<Expression *> _body;

    public:
        StatementExpression() = default;

        void addBody(Expression *expr) { _body.push_back(expr); }
        std::vector<Expression *> getBody() const { return _body; }

        llvm::Value *codegen(Context *context) override;
    };

} // namespace weasel
