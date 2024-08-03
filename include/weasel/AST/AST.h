#pragma once

#include <string>
#include <vector>
#include <cassert>

#include <glog/logging.h>

#include <weasel/Lexer/Token.h>
#include <weasel/Type/Type.h>
#include <weasel/Basic/Cast.h>
#include <weasel/Basic/Error.h>
#include <weasel/Basic/Codegen.h>
#include <weasel/AST/LInkage.h>

using namespace std;
using namespace weasel;

// Expression Base Type
namespace weasel
{
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

    // Access Type
    enum class AccessID
    {
        Load,
        Allocation,
    };

    // Expression
    class Expression
    {
        VIRTUAL_CODEGEN_EXPRESSION

    public:
        Expression() : _token(Token::create()) {}
        explicit Expression(const Token &token) : _token(token) {}
        Expression(const Token &token, Error &error) : _token(token), _error(error) {}
        Expression(const Token &token, TypeHandle type, bool isConstant = false) : _token(token), _type(type), _isConstant(isConstant) {}

        Token getToken() const { return _token; }
        TypeHandle &getType() { return _type; }

        void setToken(const Token &token) { _token = token; }
        void setType(TypeHandle type) { _type = type; }
        bool isNoType() const { return _type == nullptr; }

        // Check for all possible expression
        // for better handling difficult expression
        // like an expression that need last allocation
        bool isCompoundExpression() const;
        bool isStructExpression() const;
        bool isFieldExpression() const;
        bool isNilExpression() const;
        bool isFunctionExpression() const;
        bool isLambdaExpression() const;

        bool isConstant() const
        {
            return _isConstant;
        }

        void setAccess(AccessID accessID) { _accessID = accessID; }
        AccessID getAccess() const { return _accessID; }
        bool isAccessLoad() const { return _accessID == AccessID::Load; }
        bool isAccessAllocation() const { return _accessID == AccessID::Allocation; }

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
        EMPTY_CODEGEN_EXPRESSION

    public:
        ErrorExpression() = default;
        ErrorExpression(const Token token, Error error) : Expression(token, error) {}
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
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        ReturnExpression(const Token &token, ExpressionHandle value) : Expression(token), _value(value) {}
        ReturnExpression(const Token &token, TypeHandle type) : Expression(token, type) {}

        ExpressionHandle &getValue() { return _value; }

    private:
        ExpressionHandle _value;
    };

    // Return Expression
    class BreakExpression : public Expression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        BreakExpression(Token token, Expression *value) : Expression(token), _value(value) {}

        void setValue(ExpressionHandle expr) { _value = expr; }
        ExpressionHandle &getValue() { return _value; }

    private:
        ExpressionHandle _value;
    };

    // Return Expression
    class ContinueExpression : public Expression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        ContinueExpression(Token token, ExpressionHandle value) : Expression(token), _value(value) {}

        void setValue(ExpressionHandle expr) { _value = expr; }
        ExpressionHandle getValue() const { return _value; }

    private:
        ExpressionHandle _value;
    };

    // Variable Expression
    class VariableExpression : public Expression
    {
        OVERRIDE_CODEGEN_EXPRESSION

        DECLARATION_EXPRESSION

    public:
        VariableExpression(Token token, string identifier) : Expression(token), _identifier(identifier) {}
        VariableExpression() {}

        void setIdentifier(string identifier) { _identifier = identifier; }
        string getIdentifier() const { return _identifier; }

    private:
        string _identifier;
    };

    // Call Expression
    class CallExpression : public VariableExpression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        explicit CallExpression(Token token, string identifier) : VariableExpression(token, identifier) {}

        vector<ExpressionHandle> &getArguments() { return _args; }

        void setFunction(FunctionHandle fun) { _fun = fun; }
        FunctionHandle getFunction() { return _fun; }

    private:
        FunctionHandle _fun;
        vector<ExpressionHandle> _args;
    };

    class ArrayExpression : public VariableExpression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        ArrayExpression() {}

        void setIndexExpression(ExpressionHandle expr) { _indexExpr = expr; }
        Expression *getIndex() { return _indexExpr.get(); }

    private:
        ExpressionHandle _indexExpr;
    };

    // Struct Expression
    class StructExpression : public VariableExpression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        class StructField
        {
        private:
            string _identifier;
            ExpressionHandle _value;

        public:
            StructField(const string &identifier, ExpressionHandle expr) : _identifier(identifier), _value(expr) {}
            ~StructField() = default;

            string getIdentifier() const { return _identifier; }
            ExpressionHandle getValue() { return _value; }
            bool isEmptyIdentifier() const { return _identifier.empty(); }
        };

        using StructFieldHandle = shared_ptr<StructField>;

    public:
        StructExpression(Token token, string identifier) : VariableExpression(token, identifier) {}

        vector<StructFieldHandle> &getFields() { return _fields; }

        void setPreferConstant(bool v) { _isPreferConstant = v; }
        bool getIsPreferConstant() const { return _isPreferConstant; }

        void setAlloc(llvm::Value *alloc) { _alloc = alloc; }
        llvm::Value *getAlloc() const { return _alloc; }

    private:
        vector<StructFieldHandle> _fields;
        bool _isPreferConstant = false;

        llvm::Value *_alloc;
    };

    // FieldExpression
    class FieldExpression : public VariableExpression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        explicit FieldExpression(Token token, string identifier) : VariableExpression(token, identifier) {}

        void setParentField(ExpressionHandle expr) { _parentField = expr; }
        ExpressionHandle getParentField() { return _parentField; }

    private:
        ExpressionHandle _parentField;
    };

    // Method Call
    class MethodCallExpression : public Expression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        MethodCallExpression() = default;

        void setImplExpression(ExpressionHandle implExpression) { _implExpression = implExpression; }
        void setFunction(Function *fun) { _fun = fun; }

        vector<ExpressionHandle> getArguments() const { return _args; }
        ExpressionHandle getImplExpression() { return _implExpression; }
        Function *getFunction() const { return _fun; }

    private:
        Function *_fun;
        ExpressionHandle _implExpression;
        vector<ExpressionHandle> _args;
    };

    // Number Literal Expression
    class NumberLiteralExpression : public LiteralExpression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        NumberLiteralExpression(const Token &token, long long value, unsigned width = 32) : LiteralExpression(token, Type::getIntegerType(width)), _value(value) {}

        long long getValue() const { return _value; }

    private:
        long long _value; // 64 bit(8 bytes)
    };

    // Float Literal Expression
    class FloatLiteralExpression : public LiteralExpression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        FloatLiteralExpression(Token token, double value) : LiteralExpression(token, Type::getFloatType()), _value(value) {}

        float getValue() const { return _value; }

    private:
        float _value; // 32 bit(4 bytes)
    };

    // Float Literal Expression
    class DoubleLiteralExpression : public LiteralExpression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        DoubleLiteralExpression(Token token, double value) : LiteralExpression(token, Type::getDoubleType()), _value(value) {}

        double getValue() const { return _value; }

    private:
        double _value; // 64 bit(8 bytes)
    };

    // Boolean Literal Expression
    class BoolLiteralExpression : public LiteralExpression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        BoolLiteralExpression(Token token, bool value) : LiteralExpression(token, Type::getIntegerType(1)), _value(value) {}

        bool getValue() const { return _value; }

    private:
        bool _value;
    };

    // Character Literal Expression
    class CharLiteralExpression : public LiteralExpression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        CharLiteralExpression(Token token, char value) : LiteralExpression(token, Type::getIntegerType(8)), _value(value) {}

        char getValue() const { return _value; }

    private:
        char _value;
    };

    // String Literal Expression
    class StringLiteralExpression : public LiteralExpression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        StringLiteralExpression(Token token, const string &value) : LiteralExpression(token, Type::getArrayType(Type::getIntegerType(8, false), value.size())), _value(value) {}

        string getValue() const { return _value; }

    private:
        string _value;
    };

    // Nil Literal Expression
    class NilLiteralExpression : public LiteralExpression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        NilLiteralExpression(Token token) : LiteralExpression(token, Type::getPointerType(Type::getVoidType())) {}
    };

    // Array Expression
    class ArrayLiteralExpression : public Expression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        ArrayLiteralExpression() = default;

        vector<ExpressionHandle> &getItems() { return _items; }

    private:
        vector<ExpressionHandle> _items;
    };

    // Type Casting Operator Expression
    class TypeCastExpression : public Expression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        TypeCastExpression(Token op, TypeHandle type, ExpressionHandle rhs) : Expression(op, type), _rhs(rhs) {}

        ExpressionHandle getValue() { return _rhs; }

    private:
        ExpressionHandle _rhs;
    };

    // Binary Operator Expression
    class ArithmeticExpression : public Expression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        ArithmeticExpression(Token op, ExpressionHandle lhs, ExpressionHandle rhs) : Expression(op), _lhs(lhs), _rhs(rhs) {}
        ArithmeticExpression(Token op) : Expression(op) {}
        ArithmeticExpression() = default;

        void setLHS(ExpressionHandle lhs) { _lhs = lhs; }
        void setRHS(ExpressionHandle rhs) { _rhs = rhs; }

        Token getOperator() const { return getToken(); }
        ExpressionHandle getLHS() { return _lhs; }
        ExpressionHandle getRHS() { return _rhs; }

    private:
        ExpressionHandle _lhs;
        ExpressionHandle _rhs;
    };

    // Binary Operator Expression
    class LogicalExpression : public Expression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        LogicalExpression(Token op, ExpressionHandle lhs, ExpressionHandle rhs) : Expression(op), _lhs(lhs), _rhs(rhs) {}
        LogicalExpression(Token op) : Expression(op) {}

        void setLHS(ExpressionHandle lhs) { _lhs = lhs; }
        void setRHS(ExpressionHandle rhs) { _rhs = rhs; }

        Token getOperator() const { return getToken(); }
        Expression *getLHS() { return _lhs.get(); }
        Expression *getRHS() { return _rhs.get(); }

    private:
        ExpressionHandle _lhs;
        ExpressionHandle _rhs;
    };

    // Assignment Operator Expression
    class AssignmentExpression : public Expression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        AssignmentExpression(Token op, ExpressionHandle lhs, ExpressionHandle rhs) : Expression(op), _lhs(lhs), _rhs(rhs) {}
        AssignmentExpression(Token op) : Expression(op) {}

        void setLHS(ExpressionHandle lhs) { _lhs = lhs; }
        void setRHS(ExpressionHandle rhs) { _rhs = rhs; }

        Token getOperator() const { return getToken(); }
        ExpressionHandle getLHS() { return _lhs; }
        ExpressionHandle getRHS() { return _rhs; }

    private:
        ExpressionHandle _lhs;
        ExpressionHandle _rhs;
    };

    // Comparison Operator Expression
    class ComparisonExpression : public Expression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        ComparisonExpression(Token op, ExpressionHandle lhs, ExpressionHandle rhs) : Expression(op, Type::getBoolType()), _lhs(lhs), _rhs(rhs) {}

        void setLHS(ExpressionHandle lhs) { _lhs = lhs; }
        void setRHS(ExpressionHandle rhs) { _rhs = rhs; }

        Token getOperator() const { return getToken(); }
        ExpressionHandle getLHS() { return _lhs; }
        ExpressionHandle getRHS() { return _rhs; }

    private:
        ExpressionHandle _lhs;
        ExpressionHandle _rhs;
    };

    // Unary Operator Expression
    class UnaryExpression : public Expression
    {
        OVERRIDE_CODEGEN_EXPRESSION

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
        UnaryExpression(Token token, Operator op, ExpressionHandle value) : Expression(token), _value(value), _op(op) {}
        UnaryExpression(Token token, Operator op) : Expression(token), _op(op) {}

        void setValue(ExpressionHandle value) { _value = value; }
        ExpressionHandle getValue() { return _value; }

        Operator getOperator() const { return _op; }

    private:
        ExpressionHandle _value;
        Operator _op;
    };

    // Declaration Statement
    class DeclarationStatement : public VariableExpression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        DeclarationStatement() = default;
        DeclarationStatement(Token token) : VariableExpression(token, token.getValue()) {}

        void setQualifier(Qualifier qualifier) { _qualifier = qualifier; }
        void setValue(ExpressionHandle value) { _value = value; }

        Qualifier getQualifier() const { return _qualifier; }
        ExpressionHandle getValue() { return _value; }

    protected:
        Qualifier _qualifier;
        ExpressionHandle _value;
    };

    // Statement Expression
    class CompoundStatement : public Expression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        CompoundStatement() = default;

        void insertBody(int pos, ExpressionHandle expr) { _body.insert(_body.begin() + pos, expr); }
        vector<ExpressionHandle> &getBody() { return _body; }

    private:
        vector<ExpressionHandle> _body;
    };

    class ConditionStatement : public Expression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        ConditionStatement() = default;

        vector<ExpressionHandle> &getConditions() { return _conditions; }
        vector<CompoundStatementHandle> &getStatements() { return _statements; }
        bool isElseExist() const { return _conditions.size() < _statements.size(); }

    private:
        vector<ExpressionHandle> _conditions;
        vector<CompoundStatementHandle> _statements;
    };

    class LoopingStatement : public Expression
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        LoopingStatement(const Token &token) : Expression(token) {}

        vector<ExpressionHandle> &getConditions() { return _conditions; }

        void setBody(CompoundStatementHandle expr) { _body = expr; }
        CompoundStatementHandle &getBody() { return _body; }

        bool isInfinityCondition() const { return _conditions.empty(); }
        bool isSingleCondition() const { return _conditions.size() == 1; }

    private:
        vector<ExpressionHandle> _conditions;
        CompoundStatementHandle _body;
    };

    // Argument Expression
    class ArgumentExpression : public DeclarationStatement
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        ArgumentExpression() = default;
    };

    // Global Variable
    class GlobalVariable : public DeclarationStatement
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        GlobalVariable() = default;
    };

    // TODO: Linkage public or private
    // Global Value
    class GlobalObject : public Expression
    {
    public:
        GlobalObject(const Token &token, const string &identifier, TypeHandle type) : Expression(token, type), _identifier(identifier) {}
        GlobalObject() = default;

        void setIdentifier(string_view identifier) { _identifier = identifier; }
        string getIdentifier() const { return _identifier; }

    protected:
        // Linkage _linkage;
        string _identifier;
    };

    // Lambda Statement
    class LambdaStatement : public GlobalObject
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        LambdaStatement() {}

        void setBody(CompoundStatementHandle body) { _body = body; }
        CompoundStatementHandle getBody() { return _body; }

        void setArguments(const vector<ArgumentExpressionHandle> &arguments) { _arguments = arguments; }
        vector<ArgumentExpressionHandle> &getArguments() { return _arguments; }

        void setVararg(bool vararg) { _isVararg = vararg; }
        bool isVararg() const { return _isVararg; }

    protected:
        CompoundStatementHandle _body;
        vector<ArgumentExpressionHandle> _arguments;

        bool _isVararg = false;
    };

    // Function
    class Function : public LambdaStatement
    {
        OVERRIDE_CODEGEN_EXPRESSION

    public:
        Function() {}

        void setIsDefine(bool val) { _isDefine = val; }
        bool isDefine() const { return _isDefine; }

        void setIsInline(bool val) { _isInline = val; }
        bool isInline() const { return _isInline; }

        void setIsExtern(bool val) { _isExtern = val; }
        bool isExtern() const { return _isExtern; }

        bool isMain() const { return _identifier == "main"; }

        string getManglingName();

        void setImplStruct(StructTypeHandle structType) { _implStruct = structType; }
        bool isImplStructExist() const { return _implStruct != nullptr; }
        StructType *getImplStruct() { return _implStruct.get(); }

        void setIsStatic(bool val) { _isStatic = val; }
        bool getIsStatic() const { return _isStatic; }

        TypeHandle getReturnType()
        {
            auto type = static_pointer_cast<FunctionType>(_type);
            assert(type && "type should be a function type");

            return type->getReturnType();
        }

    protected:
        StructTypeHandle _implStruct;

        // TODO: Check if inline, extern, and static function
        bool _isDefine = false;
        bool _isInline = false;
        bool _isExtern = false;
        bool _isStatic = false;
    };
} // namespace weasel
