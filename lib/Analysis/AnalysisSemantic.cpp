#include "weasel/Analysis/AnalysisSemantic.h"

void AnalysisSemantic::onError(ExpressionHandle expr)
{
    getErrors().push_back(expr);
}

void AnalysisSemantic::semanticCheck()
{
    // TODO: Semantic for User Type
    for (auto item : getModule()->getUserTypes())
    {
    }

    // Functions
    for (auto item : getModule()->getFunctions())
    {
        if (item->isError())
        {
            auto token = item->getError().value().getToken();
            LOG(INFO) << item->getIdentifier() << " " << item->getError().value().getMessage() << " but got " << token.getValue() << " " << token.getLocation().toString();
        }

        for (auto arg : item->getArguments())
        {
            // TODO: Check for argument user type
        }

        // TODO: Check Function Return Type

        // Check Compound Statement
        compoundStatementCheck(item->getBody());
    }
}

void AnalysisSemantic::compoundStatementCheck(CompoundStatementHandle expr)
{
    if (!expr || expr->getBody().empty())
    {
        return;
    }

    for (auto item : expr->getBody())
    {
        statementCheck(item);
    }
}

void AnalysisSemantic::statementCheck(ExpressionHandle expr)
{
    if (expr->isError())
    {
        return onError(expr);
    }

    if (auto newExpr = dynamic_pointer_cast<CompoundStatement>(expr); newExpr)
    {
        compoundStatementCheck(newExpr);
    }
    else if (auto newExpr = dynamic_pointer_cast<CallExpression>(expr); newExpr)
    {
        callExpressionCheck(newExpr);
    }
}

void AnalysisSemantic::callExpressionCheck(CallExpressionHandle expr)
{
    LOG(INFO) << "Call Expression Check";
    // Check Function Call
    auto fun = getModule()->findFunction(expr->getIdentifier());
    if (!fun)
    {
        expr->setError(Errors::getInstance().functionNotDefined.withToken(expr->getToken()));
        return onError(expr);
    }

    expr->setType(fun->getType());
    expr->setFunction(fun);

    // Check Argument
    for (auto item : expr->getArguments())
    {
        // TODO: Check User Type
    }
}
