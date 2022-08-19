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

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;

        ~ReturnExpression()
        {
            delete _value;
        }
    };

    // Return Expression
    class BreakExpression : public Expression
    {
    private:
        Expression *_value;

    public:
        BreakExpression(Token token, Expression *value) : Expression(token), _value(value) {}

        Expression *getValue() const { return _value; }

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;

        ~BreakExpression()
        {
            delete _value;
        }
    };

    // Return Expression
    class ContinueExpression : public Expression
    {
    private:
        Expression *_value;

    public:
        ContinueExpression(Token token, Expression *value) : Expression(token), _value(value) {}

        Expression *getValue() const { return _value; }

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;

        ~ContinueExpression()
        {
            delete _value;
        }
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

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;

        ~CallExpression()
        {
            _args.clear();
        }
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

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;

        ~VariableExpression() {}
    };

    class ArrayExpression : public VariableExpression
    {
    private:
        Expression *_indexExpr;

    public:
        ArrayExpression(Token token, std::string identifier, Expression *indexExpr, bool addressOf = false) : VariableExpression(token, identifier, nullptr, addressOf), _indexExpr(indexExpr) {}

        Expression *getIndex() const { return _indexExpr; }

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;

        ~ArrayExpression()
        {
            delete _indexExpr;
        }
    };

    // Struct Expression
    class StructExpression : public Expression
    {
    public:
        class StructField
        {
        private:
            std::string _identifier;
            Expression *_expr;

        public:
            StructField(const std::string &identifier, Expression *expr) : _identifier(identifier), _expr(expr) {}

            inline std::string getIdentifier() const { return _identifier; }
            inline Expression *getExpression() const { return _expr; }
            inline bool isEmptyIdentifier() const { return _identifier.empty(); }

            ~StructField()
            {
                delete _expr;
            }
        };

    private:
        std::vector<StructField *> _fields;
        bool _isPreferConstant = false;

    public:
        StructExpression(Token token, StructType *type, const std::vector<StructField *> &fields) : Expression(token, type), _fields(fields) {}

        inline std::vector<StructField *> getFields() const { return _fields; }

        inline void setPreferConstant(bool v) { _isPreferConstant = v; }
        inline bool getIsPreferConstant() const { return _isPreferConstant; }

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;

        ~StructExpression()
        {
            _fields.clear();
        }
    };

    // Field Expresion
    class FieldExpression : public Expression
    {
    private:
        std::string _identifier;
        Expression *_parent;

    public:
        FieldExpression(Token token, std::string identifier, Expression *parent, Type *type) : Expression(token, type), _identifier(identifier), _parent(parent)
        {
        }

        inline std::string getField() const { return _identifier; }
        inline Expression *getParent() const { return _parent; }

    public:
        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;

        ~FieldExpression()
        {
            delete _parent;
        }
    };

} // namespace weasel
