#pragma once

#include "weasel/AST/ASTBase.h"

// Expression With Block Function PART
namespace weasel
{
    // Declaration Statement
    class DeclarationStatement : public Expression
    {
    private:
        std::string _identifier;
        Qualifier _qualifier;
        Expression *_value;

    public:
        DeclarationStatement(Token token, std::string identifier, Qualifier qualifier, Type *type, Expression *value = nullptr) : Expression(token, type), _identifier(identifier), _qualifier(qualifier), _value(value) {}

        inline Qualifier getQualifier() const { return _qualifier; }
        inline std::string getIdentifier() const { return _identifier; }
        inline Expression *getValue() const { return _value; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    // Statement Expression
    class CompoundExpression : public Expression
    {
    private:
        std::vector<Expression *> _body;

    public:
        CompoundExpression() = default;

        void addBody(Expression *expr) { _body.push_back(expr); }
        std::vector<Expression *> getBody() const { return _body; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    class ConditionStatement : public Expression
    {
    private:
        std::vector<Expression *> _conditions;
        std::vector<CompoundExpression *> _statements;

    public:
        ConditionStatement(const Token &token, const std::vector<Expression *> &conditions, const std::vector<CompoundExpression *> &statements) : Expression(token), _conditions(conditions), _statements(statements) {}

        inline std::vector<Expression *> getConditions() const { return _conditions; }
        inline std::vector<CompoundExpression *> getStatements() const { return _statements; }
        inline bool isElseExist() const { return _conditions.size() < _statements.size(); }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    class LoopingStatement : public Expression
    {
    private:
        std::vector<Expression *> _conditions;
        CompoundExpression *_body;

    public:
        LoopingStatement(const Token &token, std::vector<Expression *> conditions, CompoundExpression *body) : Expression(token), _conditions(conditions), _body(body) {}

        inline std::vector<Expression *> getConditions() const { return _conditions; }
        inline CompoundExpression *getBody() const { return _body; }
        inline bool isInfinityCondition() const { return _conditions.empty(); }
        inline bool isSingleCondition() const { return _conditions.size() == 1; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };
} // namespace weasel
