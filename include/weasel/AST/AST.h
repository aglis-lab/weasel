#pragma once

#include <string>
#include <vector>

#include <glog/logging.h>

#include "weasel/Lexer/Token.h"
#include "weasel/Type/Type.h"
#include "weasel/Basic/Cast.h"
#include "weasel/Basic/Error.h"

using namespace std;

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
    class Expression;
    class CallExpression;
    class Function;
    class ArgumentExpression;

    using ExpressionHandle = shared_ptr<Expression>;
    using CallExpressionHandle = shared_ptr<CallExpression>;
    using CompoundStatementHandle = shared_ptr<CompoundStatement>;
    using FunctionHandle = shared_ptr<Function>;
    using ArgumentExpressionHandle = shared_ptr<ArgumentExpression>;

    enum class Linkage;

    // Access Type
    enum class AccessID
    {
        Load,
        Allocation,
    };

    // Expression
    class Expression
    {
    public:
        Expression() : _token(Token::create()) {}
        explicit Expression(const Token &token) : _token(token) {}
        Expression(const Token &token, Error &error) : _token(token), _error(error) {}
        Expression(const Token &token, TypeHandle type, bool isConstant = false) : _token(token), _type(type), _isConstant(isConstant) {}

        Token getToken() const { return _token; }
        TypeHandle getType() const { return _type; }

        void setToken(const Token &token) { _token = token; }
        void setType(TypeHandle type) { _type = type; }
        bool isNoType() const { return _type == nullptr; }
        bool isCompoundExpression();

        bool isConstant() const { return _isConstant; }

        void setAccess(AccessID accessID) { _accessID = accessID; }
        AccessID getAccess() const { return _accessID; }
        bool isAccessLoad() const { return _accessID == AccessID::Load; }
        bool isAccessAllocation() const { return _accessID == AccessID::Allocation; }

        virtual ~Expression();

        virtual llvm::Value *codegen(WeaselCodegen *codegen) = 0;
        virtual void print(Printer *printer) = 0;
        virtual void printAsOperand(Printer *printer) = 0;

        void setError(Error error) { _error = error; }
        bool isError() const { return _error.has_value(); }

        bool getIsContant() const { return _isConstant; }

        optional<Error> getError() const { return _error; }

    protected:
        Token _token; // Token each expression
        TypeHandle _type;
        optional<Error> _error;

        AccessID _accessID;
        bool _isConstant;
    };

    // Error Expression
    class ErrorExpression : public Expression
    {
    public:
        ErrorExpression(const Token token, Error error) : Expression(token, error) {}
        ~ErrorExpression() = default;

        llvm::Value *codegen(WeaselCodegen *c) override {}
        void print(Printer *printer) override {}
        void printAsOperand(Printer *printer) override {}
    };

    // TODO: Linkage public or private
    // Global Value
    class GlobalObject : public Expression
    {
    public:
        GlobalObject(const Token &token, const string &identifier, TypeHandle type) : Expression(token, type), _identifier(identifier) {}
        GlobalObject() = default;

        string getIdentifier() const { return _identifier; }

        void setIdentifier(string_view identifier) { _identifier = identifier; }

    protected:
        // Linkage _linkage;
        string _identifier;
    };

    // Global Variable
    class GlobalVariable : public GlobalObject
    {
    public:
        GlobalVariable() = default;

        Expression *getValue() { return _value.get(); }

        llvm::Value *codegen(WeaselCodegen *c) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        ExpressionHandle _value;
    };

    // Literal Expression
    class LiteralExpression : public Expression
    {
    public:
        LiteralExpression(const Token &token, TypeHandle type) : Expression(token, type, true) {}
    };

    // Return Expression
    class ReturnExpression : public Expression
    {
    public:
        ReturnExpression(const Token &token, ExpressionHandle value, TypeHandle type) : Expression(token, type), _value(value) {}

        Expression *getValue() { return _value.get(); }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~ReturnExpression();

    private:
        ExpressionHandle _value;
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

    // Variable Expression
    class VariableExpression : public Expression
    {
    public:
        VariableExpression(Token token, string identifier) : Expression(token), _identifier(identifier) {}
        VariableExpression() {}

        void setIdentifier(string identifier) { _identifier = identifier; }
        string getIdentifier() const { return _identifier; }

        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~VariableExpression() {}

    private:
        string _identifier;
    };

    // Call Expression
    class CallExpression : public VariableExpression
    {
    public:
        explicit CallExpression(Token token, string identifier) : VariableExpression(token, identifier) {}

        vector<ExpressionHandle> &getArguments() { return _args; }

        void setFunction(FunctionHandle fun) { _fun = fun; }
        FunctionHandle getFunction() { return _fun; }

        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~CallExpression();

    private:
        FunctionHandle _fun;
        vector<ExpressionHandle> _args;
    };

    class ArrayExpression : public VariableExpression
    {
    public:
        ArrayExpression() {}

        void setIndexExpression(ExpressionHandle expr) { _indexExpr = expr; }
        Expression *getIndex() { return _indexExpr.get(); }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~ArrayExpression();

    private:
        ExpressionHandle _indexExpr;
    };

    // Struct Expression
    class StructExpression : public Expression
    {
    public:
        class StructField
        {
        private:
            string _identifier;
            ExpressionHandle _expr;

        public:
            StructField(const string &identifier, ExpressionHandle expr) : _identifier(identifier), _expr(expr) {}

            string getIdentifier() const { return _identifier; }
            Expression *getExpression() { return _expr.get(); }
            bool isEmptyIdentifier() const { return _identifier.empty(); }

            ~StructField();
        };

    public:
        StructExpression(Token token) : Expression(token) {}

        vector<StructField *> getFields() const { return _fields; }

        void setPreferConstant(bool v) { _isPreferConstant = v; }
        bool getIsPreferConstant() const { return _isPreferConstant; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~StructExpression();

    private:
        vector<StructField *> _fields;
        bool _isPreferConstant = false;
    };

    // Field Expresion
    // a.b : a->b
    // a.*b : a->*b
    class FieldExpression : public VariableExpression
    {
    public:
        explicit FieldExpression(Token token) : VariableExpression(token, token.getValue()) {}

        void setParentField(ExpressionHandle expr) { _parentField = expr; }
        Expression *getParentField() { return _parentField.get(); }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~FieldExpression();

    private:
        ExpressionHandle _parentField;
    };

    // Method Call
    // a.b()
    class MethodCallExpression : public Expression
    {
    public:
        using Expression::Expression;
        ~MethodCallExpression() override;

        void setImplExpression(ExpressionHandle implExpression) { _implExpression = implExpression; }
        void setFunction(Function *fun) { _fun = fun; }

        vector<ExpressionHandle> getArguments() const { return _args; }
        ExpressionHandle getImplExpression() { return _implExpression; }
        Function *getFunction() const { return _fun; }

        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        Function *_fun;
        ExpressionHandle _implExpression;
        vector<ExpressionHandle> _args;
    };

    // Number Literal Expression
    class NumberLiteralExpression : public LiteralExpression
    {
    public:
        NumberLiteralExpression(const Token &token, long long value, unsigned width = 32) : LiteralExpression(token, Type::getIntegerType(width)), _value(value) {}

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
        StringLiteralExpression(Token token, const string &value) : LiteralExpression(token, Type::getArrayType(Type::getIntegerType(8), value.size())), _value(value) {}

        string getValue() const { return _value; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        string _value;
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
        ArrayLiteralExpression(vector<Expression *> items);

    public:
        void addItem(Expression *item) { _items.push_back(item); }
        vector<Expression *> getItems() const { return _items; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~ArrayLiteralExpression();

    private:
        vector<Expression *> _items;
    };

    // Type Casting Operator Expression
    class TypeCastExpression : public Expression
    {
    public:
        TypeCastExpression(Token op, TypeHandle type, ExpressionHandle rhs) : Expression(op, type), _rhs(rhs) {}

        Expression *getExpression() { return _rhs.get(); }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        ExpressionHandle _rhs;
    };

    // Binary Operator Expression
    class ArithmeticExpression : public Expression
    {
    public:
        ArithmeticExpression(Token op, ExpressionHandle lhs, ExpressionHandle rhs) : Expression(op), _lhs(lhs), _rhs(rhs) {}
        ArithmeticExpression(Token op) : Expression(op) {}
        ArithmeticExpression() = default;

        void setLHS(ExpressionHandle lhs) { _lhs = lhs; }
        void setRHS(ExpressionHandle rhs) { _rhs = rhs; }

        Token getOperator() const { return getToken(); }
        ExpressionHandle getLHS() { return _lhs; }
        ExpressionHandle getRHS() { return _rhs; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        ExpressionHandle _lhs;
        ExpressionHandle _rhs;
    };

    // Binary Operator Expression
    class LogicalExpression : public Expression
    {
    public:
        LogicalExpression(Token op, ExpressionHandle lhs, ExpressionHandle rhs) : Expression(op), _lhs(lhs), _rhs(rhs) {}
        LogicalExpression(Token op) : Expression(op) {}

        void setLHS(ExpressionHandle lhs) { _lhs = lhs; }
        void setRHS(ExpressionHandle rhs) { _rhs = rhs; }

        Token getOperator() const { return getToken(); }
        Expression *getLHS() { return _lhs.get(); }
        Expression *getRHS() { return _rhs.get(); }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        ExpressionHandle _lhs;
        ExpressionHandle _rhs;
    };

    // Assignment Operator Expression
    class AssignmentExpression : public Expression
    {
    public:
        AssignmentExpression(Token op, ExpressionHandle lhs, ExpressionHandle rhs) : Expression(op), _lhs(lhs), _rhs(rhs) {}
        AssignmentExpression(Token op) : Expression(op) {}

        void setLHS(ExpressionHandle lhs) { _lhs = lhs; }
        void setRHS(ExpressionHandle rhs) { _rhs = rhs; }

        Token getOperator() const { return getToken(); }
        ExpressionHandle getLHS() { return _lhs; }
        ExpressionHandle getRHS() { return _rhs; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        ExpressionHandle _lhs;
        ExpressionHandle _rhs;
    };

    // Comparison Operator Expression
    class ComparisonExpression : public Expression
    {
    public:
        ComparisonExpression(Token op, ExpressionHandle lhs, ExpressionHandle rhs) : Expression(op, Type::getBoolType()), _lhs(lhs), _rhs(rhs) {}

        void setLHS(ExpressionHandle lhs) { _lhs = lhs; }
        void setRHS(ExpressionHandle rhs) { _rhs = rhs; }

        Token getOperator() const { return getToken(); }
        Expression *getLHS() { return _lhs.get(); }
        Expression *getRHS() { return _rhs.get(); }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        ExpressionHandle _lhs;
        ExpressionHandle _rhs;
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
        UnaryExpression(Token token, Operator op, ExpressionHandle rhs) : Expression(token), _rhs(rhs), _op(op) {}
        UnaryExpression(Token token, Operator op) : Expression(token), _op(op) {}

        void setRHS(ExpressionHandle rhs) { _rhs = rhs; }

        Expression *getExpression() { return _rhs.get(); }
        Operator getOperator() const { return _op; }

        ~UnaryExpression();

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

    private:
        ExpressionHandle _rhs;
        Operator _op;
    };

    // Declaration Statement
    class DeclarationStatement : public VariableExpression
    {
    public:
        DeclarationStatement() = default;
        DeclarationStatement(Token token) : VariableExpression(token, token.getValue()) {}

        void setQualifier(Qualifier qualifier) { _qualifier = qualifier; }
        void setValue(ExpressionHandle valueExpr) { _valueExpr = valueExpr; }

        Qualifier getQualifier() const { return _qualifier; }
        ExpressionHandle getValue() { return _valueExpr; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~DeclarationStatement();

    private:
        Qualifier _qualifier;
        ExpressionHandle _valueExpr;
    };

    // Statement Expression
    class CompoundStatement : public Expression
    {
    public:
        CompoundStatement() = default;

        void insertBody(int pos, ExpressionHandle expr) { _body.insert(_body.begin() + pos, expr); }
        vector<ExpressionHandle> &getBody() { return _body; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~CompoundStatement();

    private:
        vector<ExpressionHandle> _body;
    };

    class ConditionStatement : public Expression
    {
    public:
        ConditionStatement() = default;

        vector<ExpressionHandle> &getConditions() { return _conditions; }
        vector<CompoundStatementHandle> &getStatements() { return _statements; }
        bool isElseExist() const { return _conditions.size() < _statements.size(); }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~ConditionStatement();

    private:
        vector<ExpressionHandle> _conditions;
        vector<CompoundStatementHandle> _statements;
    };

    class LoopingStatement : public Expression
    {
    public:
        LoopingStatement(const Token &token, vector<Expression *> conditions, CompoundStatement *body) : Expression(token), _conditions(conditions), _body(body) {}

        vector<Expression *> getConditions() const { return _conditions; }
        CompoundStatement *getBody() const { return _body; }
        bool isInfinityCondition() const { return _conditions.empty(); }
        bool isSingleCondition() const { return _conditions.size() == 1; }

    public:
        llvm::Value *codegen(WeaselCodegen *codegen) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *printer) override;

        ~LoopingStatement();

    private:
        vector<Expression *> _conditions;
        CompoundStatement *_body;
    };

    // Argument Expression
    class ArgumentExpression : public VariableExpression
    {
    private:
    public:
        ArgumentExpression() = default;
        ~ArgumentExpression() = default;

        llvm::Value *codegen(WeaselCodegen *c) {}
        void print(Printer *printer) {}
        void printAsOperand(Printer *printer) {}
    };

    // Function
    class Function : public GlobalObject
    {
    public:
        Function() = default;

        ~Function() override;

        CompoundStatementHandle getBody() { return _body; }
        void setBody(CompoundStatementHandle body) { _body = body; }

        void setIsDefine(bool val) { _isDefine = val; }
        bool isDefine() const { return _isDefine; }

        void setIsInline(bool val) { _isInline = val; }
        bool isInline() const { return _isInline; }

        void setIsExtern(bool val) { _isExtern = val; }
        bool isExtern() const { return _isExtern; }

        bool isMain() const { return this->_identifier == "main"; }

        void setArguments(const vector<ArgumentExpressionHandle> &arguments) { _arguments = arguments; }
        vector<ArgumentExpressionHandle> &getArguments() { return _arguments; }

        string getManglingName();

        void setImplStruct(StructTypeHandle structType) { _implStruct = structType; }
        bool isImplStructExist() const { return _implStruct != nullptr; }
        StructType *getImplStruct() { return _implStruct.get(); }

        void setIsStatic(bool val) { _isStatic = val; }
        bool getIsStatic() const { return _isStatic; }

    public:
        llvm::Value *codegen(WeaselCodegen *c) override;
        void print(Printer *printer) override;
        void printAsOperand(Printer *) override {}

    private:
        CompoundStatementHandle _body;
        vector<ArgumentExpressionHandle> _arguments;
        StructTypeHandle _implStruct;

        // TODO: Check if inline, extern, and static function
        bool _isDefine = false;
        bool _isInline = false;
        bool _isExtern = false;
        bool _isStatic = false;
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
} // namespace weasel
