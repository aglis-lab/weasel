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

    // Field Expresion
    class FieldExpression : public Expression
    {
    private:
        std::string _identifier;
        Qualifier _qualifier;
        Expression *_value;

    public:
        FieldExpression(Token token, std::string identifier, Qualifier qualifier, Type *type, Expression *value = nullptr) : Expression(token, type), _identifier(identifier), _qualifier(qualifier), _value(value) {}

        inline Qualifier getQualifier() const { return _qualifier; }
        inline std::string getIdentifier() const { return _identifier; }
        inline Expression *getValue() const { return _value; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Struct Expression
    class StructExpression : public Expression
    {
    public:
        class StructField
        {
        private:
        public:
            StructField() {}
        };

    private:
        StructType *_type;
        std::vector<StructField> _fields;

    public:
        StructExpression();

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

} // namespace weasel
