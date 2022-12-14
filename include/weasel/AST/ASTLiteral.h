#pragma once

#include "weasel/AST/ASTBase.h"

namespace weasel
{
    // Number Literal Expression
    class NumberLiteralExpression : public LiteralExpression
    {
    private:
        long long _value; // 64 bit(8 bytes)

    public:
        NumberLiteralExpression(Token token, long long value, unsigned width = 32) : LiteralExpression(token, Type::getIntegerType(width)), _value(value) {}

        long long getValue() const { return _value; }

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Float Literal Expression
    class FloatLiteralExpression : public LiteralExpression
    {
    private:
        float _value; // 32 bit(4 bytes)

    public:
        FloatLiteralExpression(Token token, double value) : LiteralExpression(token, Type::getFloatType()), _value(value) {}

        float getValue() const { return _value; }

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Float Literal Expression
    class DoubleLiteralExpression : public LiteralExpression
    {
    private:
        double _value; // 64 bit(8 bytes)

    public:
        DoubleLiteralExpression(Token token, double value) : LiteralExpression(token, Type::getDoubleType()), _value(value) {}

        double getValue() const { return _value; }

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Boolean Literal Expression
    class BoolLiteralExpression : public LiteralExpression
    {
    private:
        bool _value;

    public:
        BoolLiteralExpression(Token token, bool value) : LiteralExpression(token, Type::getIntegerType(1)), _value(value) {}

        bool getValue() const { return _value; }

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Character Literal Expression
    class CharLiteralExpression : public LiteralExpression
    {
    private:
        char _value;

    public:
        CharLiteralExpression(Token token, char value) : LiteralExpression(token, Type::getIntegerType(8)), _value(value) {}

        char getValue() const { return _value; }

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // String Literal Expression
    class StringLiteralExpression : public LiteralExpression
    {
    private:
        std::string _value;

    public:
        StringLiteralExpression(Token token, const std::string &value) : LiteralExpression(token, Type::getArrayType(Type::getIntegerType(8), value.size())), _value(value) {}

        std::string getValue() const { return _value; }

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Nil Literal Expression
    class NilLiteralExpression : public LiteralExpression
    {
    public:
        NilLiteralExpression(Token token) : LiteralExpression(token, Type::getPointerType(Type::getIntegerType(8, false))) {}

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
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

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;

        ~ArrayLiteralExpression()
        {
            _items.clear();
        }
    };

} // namespace weasel
