#pragma once

#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include "llvm/IR/Value.h"
#include "weasel/lex/token.h"

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

//

//

// Expression Base Type
namespace weasel
{
    // Expression
    class Expression
    {
    protected:
        Token *_token; // Token each expression

    public:
        Expression() = default;
        explicit Expression(Token *token) : _token(token) {}

        Token *getToken() const { return _token; }

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
        LiteralExpression(Token *token, LiteralType type, unsigned width, unsigned size = 1) : Expression(token), _type(type), _width(width), _size(size) {}
    };

} // namespace weasel

//

//

// Expression Without Block PART
namespace weasel
{
    // Return Expression
    class ReturnExpression : public Expression
    {
    private:
        Expression *_value;

    public:
        ReturnExpression(Token *token, Expression *value) : Expression(token), _value(value) {}

        Expression *getValue() const { return _value; }

        llvm::Value *codegen(Context *context) override;
    };

    // Call Expression
    class CallExpression : public Expression
    {
        std::string _identifier;
        std::vector<Expression *> _args;

    public:
        CallExpression(Token *token, std::string identifier, std::vector<Expression *> args) : Expression(token), _identifier(identifier), _args(args) {}

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
        VariableExpression(Token *token, std::string identifier, bool addressOf = false) : Expression(token), _identifier(identifier), _addressOf(addressOf) {}

        std::string getIdentifier() const { return _identifier; }
        bool isAddressOf() const { return _addressOf; }

        llvm::Value *codegen(Context *context) override;
    };

    class ArrayExpression : public VariableExpression
    {
    private:
        Expression *_indexExpr;

    public:
        ArrayExpression(Token *token, std::string identifier, Expression *indexExpr, bool addressOf = false) : VariableExpression(token, identifier, addressOf), _indexExpr(indexExpr) {}

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
        DeclarationExpression(Token *token, std::string identifier, Qualifier qualifier, llvm::Type *type = nullptr, Expression *value = nullptr) : Expression(token), _identifier(identifier), _qualifier(qualifier), _type(type), _value(value) {}

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
        NumberLiteralExpression(Token *token, long long value, unsigned width = 32) : LiteralExpression(token, LiteralType::LiteralNumber, width), _value(value) {}

        long long getValue() const { return _value; }

        llvm::Value *codegen(Context *context) override;
    };

    // Boolean Literal Expression
    class BoolLiteralExpression : public LiteralExpression
    {
    private:
        bool _value;

    public:
        BoolLiteralExpression(Token *token, bool value) : LiteralExpression(token, LiteralType::LiteralBool, 1), _value(value) {}

        bool getValue() const { return _value; }

        llvm::Value *codegen(Context *context) override { return nullptr; }
    };

    // String Literal Expression
    class StringLiteralExpression : public LiteralExpression
    {
    private:
        std::string _value;

    public:
        StringLiteralExpression(Token *token, const std::string &value) : LiteralExpression(token, LiteralType::LiteralString, 8, value.size()), _value(value) {}

        std::string getValue() const { return _value; }

        llvm::Value *codegen(Context *context) override;
    };

    // Nil Literal Expression
    class NilLiteralExpression : public LiteralExpression
    {
    public:
        explicit NilLiteralExpression(Token *token) : LiteralExpression(token, LiteralType::LiteralNil, 64) {}

        llvm::Value *codegen(Context *context) override;
    };

    // Binary Operator Expression
    class BinaryOperatorExpression : public Expression
    {
    private:
        Token *_operator;
        Expression *_lhs;
        Expression *_rhs;

    public:
        BinaryOperatorExpression(Token *op, Expression *lhs, Expression *rhs) : _operator(op), _lhs(lhs), _rhs(rhs) {}

        Token *getOperator() const { return _operator; }
        Expression *getLHS() const { return _lhs; }
        Expression *getRHS() const { return _rhs; }

        llvm::Value *codegen(Context *context) override;
    };

    // Unary Operator Expression
    class UnaryOperatorExpression : public Expression
    {
    private:
        Token *_lhs;
        Expression *_rhs;

    public:
        UnaryOperatorExpression(Token *lhs, Expression *rhs) : _lhs(lhs), _rhs(rhs) {}

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

//

//

// Function PART
namespace weasel
{
    // Func Arg
    class FunctionArgument
    {
    private:
        Token *_token;
        std::string _argName;
        llvm::Type *_type;

    public:
        FunctionArgument(Token *token, std::string argName, llvm::Type *type) : _token(token), _argName(argName), _type(type) {}

        Token *getToken() const { return _token; }
        llvm::Type *getArgumentType() const { return _type; }
        std::string getArgumentName() const { return _argName; }
    };

    // Func Type
    class FunctionType
    {
    private:
        std::vector<FunctionArgument *> _args;
        llvm::Type *_retType;
        bool _isVararg;

    public:
        FunctionType(llvm::Type *returnType, std::vector<FunctionArgument *> args, bool vararg) : _args(args), _retType(returnType), _isVararg(vararg) {}

        std::vector<FunctionArgument *> getArgs() const { return _args; }
        llvm::Type *getReturnType() const { return _retType; }

        bool getIsVararg() const { return _isVararg; }
    };

    // Func
    class Function
    {
    private:
        std::string _identifier;
        FunctionType *_funcTy;
        StatementExpression *_body;
        bool _isDefine = false;
        bool _isInline = false;
        ParallelType _parallelType;

    public:
        Function(std::string identifier, FunctionType *funcTy, ParallelType parallelTy = ParallelType::None) : _identifier(identifier), _funcTy(funcTy), _parallelType(parallelTy) {}

        std::string getIdentifier() const { return _identifier; }
        FunctionType *getFunctionType() const { return _funcTy; }
        std::vector<FunctionArgument *> getArgs() const { return _funcTy->getArgs(); }

        StatementExpression *getBody() const { return _body; }
        void setBody(StatementExpression *body) { _body = body; }

        void setIsDefine(bool val) { _isDefine = val; }
        bool isDefine() const { return _isDefine; }

        void setIsInline(bool val) { _isInline = val; }
        bool isInline() const { return _isInline; }

        void setParallelType(ParallelType parallelType) { _parallelType = parallelType; }
        ParallelType getParallelType() const { return _parallelType; }

    public:
        llvm::Function *codegen(Context *c);
    };

} // namespace weasel
