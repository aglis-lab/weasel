#pragma once

#include "weasel/AST/ASTBase.h"

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
        void debug(int shift) override;
    };

    class ConditionStatement : public Expression
    {
    private:
        std::vector<Expression *> _conditions;
        std::vector<StatementExpression *> _statements;

    public:
        ConditionStatement(const Token &token, const std::vector<Expression *> &conditions, const std::vector<StatementExpression *> &statements) : Expression(token), _conditions(conditions), _statements(statements) {}

        inline std::vector<Expression *> getConditions() const { return _conditions; }
        inline std::vector<StatementExpression *> getStatements() const { return _statements; }
        inline bool isElseExist() const { return _conditions.size() < _statements.size(); }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };

    class LoopingStatement : public Expression
    {
    private:
        std::vector<Expression *> _conditions;
        StatementExpression *_body;

    public:
        LoopingStatement(const Token &token, std::vector<Expression *> conditions, StatementExpression *body) : Expression(token), _conditions(conditions), _body(body) {}

        inline std::vector<Expression *> getConditions() const { return _conditions; }
        inline StatementExpression *getBody() const { return _body; }
        inline bool isInfinityCondition() const { return _conditions.empty(); }
        inline bool isSingleCondition() const { return _conditions.size() == 1; }

        llvm::Value *codegen(Context *context) override;
        void debug(int shift) override;
    };
} // namespace weasel
