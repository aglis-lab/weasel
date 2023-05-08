#pragma once

// Include Standard Library
#include <string>
#include <vector>

#include <weasel/Lexer/Token.h>
#include <weasel/Type/Type.h>
#include <weasel/Basic/Cast.h>

#include <glog/logging.h>

namespace llvm
{
    class Value;
} // namespace llvm

// Expression Base Type
namespace weasel
{
    class Printer;
    class WeaselCodegen;
    class CompoundStatement;

    enum class Linkage;
    enum class MetaID;

    // Expression
    class Expression
    {
    public:
        Expression() : _token(Token::create()) {}
        Expression(Token token) : _token(token) {}
        Expression(Token token, Type *type) : _token(token), _type(type) {}

        Token getToken() const;
        Type *getType() const;

        void setType(Type *type);
        bool isNoType() const;
        bool isCompoundExpression();

    public:
        // TODO: Rewrite all of this logic
        void addMeta(MetaID meta) { _metas.push_back(meta); }
        bool isRHS();
        bool isLHS();

        virtual ~Expression();

    public:
        virtual llvm::Value *codegen(WeaselCodegen *codegen) = 0;
        virtual void print(Printer *printer) = 0;
        virtual void printAsOperand(Printer *printer) = 0;

    protected:
        Token _token; // Token each expression
        Type *_type;
        std::vector<MetaID> _metas;
    };

    // Global Value
    class GlobalObject : public Expression
    {
    public:
        GlobalObject(Token token, const std::string &identifier, Type *type) : Expression(token, type), _identifier(identifier) {}

        std::string getIdentifier() const { return _identifier; }

    protected:
        Linkage _linkage;
        std::string _identifier;
    };

    // Global Variable
    class GlobalVariable : public GlobalObject
    {
    };

    // Function
    class Function : public GlobalObject
    {
    public:
        Function(std::string identifier, Type *type, std::vector<ArgumentType *> arguments) : GlobalObject(Token::create(), identifier, type), _arguments(arguments) {}

        CompoundStatement *getBody() const { return _body; }
        void setBody(CompoundStatement *body) { _body = body; }

        void setIsDefine(bool val) { _isDefine = val; }
        bool isDefine() const { return _isDefine; }

        void setIsInline(bool val) { _isInline = val; }
        bool isInline() const { return _isInline; }

        void setParallel(bool val) { _isParallel = val; }
        bool getParallel() const { return _isParallel; }

        void setArguments(std::vector<ArgumentType *> arguments) { _arguments = arguments; }
        std::vector<ArgumentType *> getArguments() const { return _arguments; }

    public:
        llvm::Value *codegen(WeaselCodegen *c) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override{};

    public:
        ~Function();

    private:
        CompoundStatement *_body;
        std::vector<ArgumentType *> _arguments;

        bool _isDefine = false;
        bool _isInline = false;
        bool _isParallel = false;
    };

    // Literal Expression
    class LiteralExpression : public Expression
    {
    public:
        LiteralExpression(Token token, Type *type) : Expression(token, type) {}
    };

    // Return Expression
    class ReturnExpression : public Expression
    {
    public:
        ReturnExpression(Token token, Expression *value) : Expression(token, value->getType()), _value(value) {}
        ReturnExpression(Token token, Expression *value, Type *type) : Expression(token, type), _value(value) {}

        Expression *getValue() const { return _value; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~ReturnExpression();

    private:
        Expression *_value;
    };

    // Return Expression
    class BreakExpression : public Expression
    {
    public:
        BreakExpression(Token token, Expression *value) : Expression(token), _value(value) {}

        Expression *getValue() const { return _value; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~BreakExpression();

    private:
        Expression *_value;
    };

    // Return Expression
    class ContinueExpression : public Expression
    {
    public:
        ContinueExpression(Token token, Expression *value) : Expression(token), _value(value) {}

        Expression *getValue() const { return _value; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~ContinueExpression();

    private:
        Expression *_value;
    };

    // Call Expression
    class CallExpression : public Expression
    {
    public:
        CallExpression(Token token, Function *fun, std::vector<Expression *> args) : Expression(token, fun->getType()), _fun(fun), _args(args) {}

        std::vector<Expression *> getArguments() const { return _args; }
        Function *getFunction() const { return _fun; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~CallExpression();

    private:
        Function *_fun;
        std::vector<Expression *> _args;
    };

    // Variable Expression
    class VariableExpression : public Expression
    {
    public:
        VariableExpression(Token token, std::string identifier, Type *type) : Expression(token, type), _identifier(identifier) {}

        std::string getIdentifier() const { return _identifier; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~VariableExpression() {}

    private:
        std::string _identifier;
    };

    class ArrayExpression : public VariableExpression
    {
    public:
        ArrayExpression(Token token, std::string identifier, Expression *indexExpr, Type *type) : VariableExpression(token, identifier, type), _indexExpr(indexExpr) {}

        Expression *getIndex() const { return _indexExpr; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~ArrayExpression();

    private:
        Expression *_indexExpr;
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

            std::string getIdentifier() const { return _identifier; }
            Expression *getExpression() const { return _expr; }
            bool isEmptyIdentifier() const { return _identifier.empty(); }

            ~StructField();
        };

    public:
        StructExpression(Token token, StructType *type, const std::vector<StructField *> &fields) : Expression(token, type), _fields(fields) {}

        std::vector<StructField *> getFields() const { return _fields; }

        void setPreferConstant(bool v) { _isPreferConstant = v; }
        bool getIsPreferConstant() const { return _isPreferConstant; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override{};

        ~StructExpression();

    private:
        std::vector<StructField *> _fields;
        bool _isPreferConstant = false;
    };

    // Field Expresion
    // a.b : a->b
    // a.*b : a->*b
    class FieldExpression : public Expression
    {
    public:
        FieldExpression(Token token, std::string identifier, Expression *parent, Type *type) : Expression(token, type), _identifier(identifier), _parentField(parent) {}

        std::string getField() const { return _identifier; }
        Expression *getParentField() const { return _parentField; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~FieldExpression();

    private:
        std::string _identifier;
        Expression *_parentField;
    };

    // Number Literal Expression
    class NumberLiteralExpression : public LiteralExpression
    {
    public:
        NumberLiteralExpression(Token token, long long value, unsigned width = 32) : LiteralExpression(token, Type::getIntegerType(width)), _value(value) {}

        long long getValue() const { return _value; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        long long _value; // 64 bit(8 bytes)
    };

    // Float Literal Expression
    class FloatLiteralExpression : public LiteralExpression
    {
    public:
        FloatLiteralExpression(Token token, double value) : LiteralExpression(token, Type::getFloatType()), _value(value) {}

        float getValue() const { return _value; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        float _value; // 32 bit(4 bytes)
    };

    // Float Literal Expression
    class DoubleLiteralExpression : public LiteralExpression
    {
    public:
        DoubleLiteralExpression(Token token, double value) : LiteralExpression(token, Type::getDoubleType()), _value(value) {}

        double getValue() const { return _value; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        double _value; // 64 bit(8 bytes)
    };

    // Boolean Literal Expression
    class BoolLiteralExpression : public LiteralExpression
    {
    public:
        BoolLiteralExpression(Token token, bool value) : LiteralExpression(token, Type::getIntegerType(1)), _value(value) {}

        bool getValue() const { return _value; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        bool _value;
    };

    // Character Literal Expression
    class CharLiteralExpression : public LiteralExpression
    {
    public:
        CharLiteralExpression(Token token, char value) : LiteralExpression(token, Type::getIntegerType(8)), _value(value) {}

        char getValue() const { return _value; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        char _value;
    };

    // String Literal Expression
    class StringLiteralExpression : public LiteralExpression
    {
    public:
        StringLiteralExpression(Token token, const std::string &value) : LiteralExpression(token, Type::getArrayType(Type::getIntegerType(8), value.size())), _value(value) {}

        std::string getValue() const { return _value; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        std::string _value;
    };

    // Nil Literal Expression
    class NilLiteralExpression : public LiteralExpression
    {
    public:
        NilLiteralExpression(Token token) : LiteralExpression(token, Type::getPointerType(Type::getIntegerType(8, false))) {}

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;
    };

    // Array Expression
    class ArrayLiteralExpression : public Expression
    {
    public:
        ArrayLiteralExpression() = default;
        ArrayLiteralExpression(std::vector<Expression *> items);

    public:
        void addItem(Expression *item) { _items.push_back(item); }
        std::vector<Expression *> getItems() const { return _items; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~ArrayLiteralExpression();

    private:
        std::vector<Expression *> _items;
    };

    // Type Casting Operator Expression
    class TypeCastExpression : public Expression
    {
    public:
        TypeCastExpression(Token op, Type *type, Expression *rhs) : Expression(op, type), _rhs(rhs) {}

        Expression *getExpression() const { return _rhs; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        Expression *_rhs;
    };

    // Binary Operator Expression
    class ArithmeticExpression : public Expression
    {
    public:
        ArithmeticExpression(Token op, Expression *lhs, Expression *rhs) : Expression(op, lhs->getType()), _lhs(lhs), _rhs(rhs) {}

        Token getOperator() const { return getToken(); }
        Expression *getLHS() const { return _lhs; }
        Expression *getRHS() const { return _rhs; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        Expression *_lhs;
        Expression *_rhs;
    };

    // Binary Operator Expression
    class LogicalExpression : public Expression
    {
    public:
        LogicalExpression(Token op, Expression *lhs, Expression *rhs) : Expression(op, lhs->getType()), _lhs(lhs), _rhs(rhs) {}

        Token getOperator() const { return getToken(); }
        Expression *getLHS() const { return _lhs; }
        Expression *getRHS() const { return _rhs; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        Expression *_lhs;
        Expression *_rhs;
    };

    // Assignment Operator Expression
    class AssignmentExpression : public Expression
    {
    public:
        AssignmentExpression(Token op, Expression *lhs, Expression *rhs) : Expression(op, lhs->getType()), _lhs(lhs), _rhs(rhs) {}

        Token getOperator() const { return getToken(); }
        Expression *getLHS() const { return _lhs; }
        Expression *getRHS() const { return _rhs; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        Expression *_lhs;
        Expression *_rhs;
    };

    // Comparison Operator Expression
    class ComparisonExpression : public Expression
    {
    public:
        ComparisonExpression(Token op, Expression *lhs, Expression *rhs) : Expression(op, Type::getBoolType()), _lhs(lhs), _rhs(rhs) {}

        Token getOperator() const { return getToken(); }
        Expression *getLHS() const { return _lhs; }
        Expression *getRHS() const { return _rhs; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        Expression *_lhs;
        Expression *_rhs;
    };

    // Unary Operator Expression
    class UnaryExpression : public Expression
    {
    public:
        enum Operator
        {
            Borrow,      // & Expression
            Dereference, // * Expression
            Negative,    // - Expression
            Positive,    // + Expression
            Not,         // ! Expression
            Negation,    // ~ Expression
        };

    public:
        UnaryExpression(Token token, Operator op, Expression *rhs) : Expression(token), _rhs(rhs), _op(op)
        {
            auto type = rhs->getType();
            if (op == Operator::Borrow)
                type = Type::getPointerType(type);
            else if (op == Operator::Dereference)
                type = type->getContainedType();
            else if (op == Operator::Not)
                type = Type::getBoolType();

            setType(type);
        }

        Expression *getExpression() const { return _rhs; }
        Operator getOperator() const { return _op; }

        ~UnaryExpression();

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        Expression *_rhs;
        Operator _op;
    };

    // Declaration Statement
    class DeclarationStatement : public Expression
    {
    public:
        DeclarationStatement(Token token, std::string identifier, Qualifier qualifier, Type *type, Expression *value = nullptr) : Expression(token, type), _identifier(identifier), _qualifier(qualifier), _value(value) {}

        Qualifier getQualifier() const { return _qualifier; }
        std::string getIdentifier() const { return _identifier; }
        Expression *getValue() const { return _value; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~DeclarationStatement();

    private:
        std::string _identifier;
        Qualifier _qualifier;
        Expression *_value;
    };

    // Statement Expression
    class CompoundStatement : public Expression
    {
    public:
        CompoundStatement() = default;

        void addBody(Expression *expr) { _body.push_back(expr); }
        void insertBody(int pos, Expression *expr) { _body.insert(_body.begin() + pos, expr); }
        std::vector<Expression *> getBody() const { return _body; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~CompoundStatement();

    private:
        std::vector<Expression *> _body;
    };

    class ConditionStatement : public Expression
    {
    public:
        ConditionStatement(const Token &token, const std::vector<Expression *> &conditions, const std::vector<CompoundStatement *> &statements) : Expression(token), _conditions(conditions), _statements(statements) {}

        std::vector<Expression *> getConditions() const { return _conditions; }
        std::vector<CompoundStatement *> getStatements() const { return _statements; }
        bool isElseExist() const { return _conditions.size() < _statements.size(); }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~ConditionStatement();

    private:
        std::vector<Expression *> _conditions;
        std::vector<CompoundStatement *> _statements;
    };

    class LoopingStatement : public Expression
    {
    public:
        LoopingStatement(const Token &token, std::vector<Expression *> conditions, CompoundStatement *body) : Expression(token), _conditions(conditions), _body(body) {}

        std::vector<Expression *> getConditions() const { return _conditions; }
        CompoundStatement *getBody() const { return _body; }
        bool isInfinityCondition() const { return _conditions.empty(); }
        bool isSingleCondition() const { return _conditions.size() == 1; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~LoopingStatement();

    private:
        std::vector<Expression *> _conditions;
        CompoundStatement *_body;
    };
} // namespace weasel

namespace weasel
{
    class WeaselCodegen;

    // Linkage //
    enum class Linkage
    {
        /// No linkage, which means that the entity is unique and
        /// can only be referred to from within its scope.
        NoLinkage = 0,

        /// Internal linkage, which indicates that the entity can
        /// be referred to from within the translation unit (but not other
        /// translation units).
        InternalLinkage,

        /// External linkage within a unique namespace.
        ///
        /// From the language perspective, these entities have external
        /// linkage. However, since they reside in an anonymous namespace,
        /// their names are unique to this translation unit, which is
        /// equivalent to having internal linkage from the code-generation
        /// point of view.
        UniqueExternalLinkage,

        /// No linkage according to the standard, but is visible from other
        /// translation units because of types defined in a function.
        VisibleNoLinkage,

        /// Internal linkage according to the Modules TS, but can be referred
        /// to from other translation units indirectly through functions and
        /// templates in the module interface.
        ModuleInternalLinkage,

        /// Module linkage, which indicates that the entity can be referred
        /// to from other translation units within the same module, and indirectly
        /// from arbitrary other translation units through functions and
        /// templates in the module interface.
        ModuleLinkage,

        /// External linkage, which indicates that the entity can
        /// be referred to from other translation units.
        ExternalLinkage
    };

    // MetaID
    enum class MetaID
    {
        RHS,
        LHS,
    };

} // namespace weasel
