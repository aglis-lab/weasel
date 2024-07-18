#include <cassert>

#include "weasel/Analysis/AnalysisSemantic.h"

void AnalysisSemantic::semanticCheck()
{
    // Struct Type
    for (auto item : getModule()->getUserTypes())
    {
        if (item->isError())
        {
            onStructError(item);
            break;
        }

        // TODO: Semantic for User Type
    }

    // Functions
    for (auto item : getModule()->getFunctions())
    {
        if (item->isError())
        {
            onError(item);
            break;
        }

        for (auto arg : item->getArguments())
        {
            // TODO: Check for argument user type
        }

        // TODO: Check Function Return Type

        // Check Compound Statement
        expressionCheck(item->getBody());
    }
}

void AnalysisSemantic::userTypeCheck(StructTypeHandle expr)
{
}

void AnalysisSemantic::expressionCheck(ExpressionHandle expr)
{
    if (!expr)
    {
        return;
    }

    if (expr->isError())
    {
        return onError(expr);
    }

    auto exprName = typeid(*expr).name();
    if (typeid(CompoundStatement).name() == exprName)
    {
        compoundStatementCheck(static_pointer_cast<CompoundStatement>(expr));
    }
    else if (typeid(CallExpression).name() == exprName)
    {
        callExpressionCheck(static_pointer_cast<CallExpression>(expr));
    }
    else if (typeid(ConditionStatement).name() == exprName)
    {
        conditionStatementChech(static_pointer_cast<ConditionStatement>(expr));
    }
    else if (typeid(DeclarationStatement).name() == exprName)
    {
        declarationStatementCheck(static_pointer_cast<DeclarationStatement>(expr));
    }
    else if (typeid(VariableExpression).name() == exprName)
    {
        variableExpressionCheck(static_pointer_cast<VariableExpression>(expr));
    }
    else if (typeid(AssignmentExpression).name() == exprName)
    {
        assignmentExpressionCheck(static_pointer_cast<AssignmentExpression>(expr));
    }
    // else if (typeid(LiteralExpression).name() == exprName)
    // {
    //     literalExpressionCheck(static_pointer_cast<LiteralExpression>(expr));
    // }
}

void AnalysisSemantic::compoundStatementCheck(CompoundStatementHandle expr)
{
    LOG(INFO) << "Compound Statement Check";

    if (!expr || expr->getBody().empty())
    {
        return;
    }

    auto lastDeclaration = getDeclarations().size();
    for (auto item : expr->getBody())
    {
        expressionCheck(item);
    }

    getDeclarations().resize(lastDeclaration);
}

void AnalysisSemantic::conditionStatementChech(ConditionStatementHandle expr)
{
    LOG(INFO) << "Condition Statement Check";

    for (auto item : expr->getConditions())
    {
        expressionCheck(item);
    }

    for (auto item : expr->getStatements())
    {
        expressionCheck(item);
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
        expressionCheck(item);
    }
}

void AnalysisSemantic::declarationStatementCheck(DeclarationStatementHandle expr)
{
    LOG(INFO) << "Declaration Statement Check";

    if (!expr->getValue())
    {
        getDeclarations().push_back(expr);

        // TODO: Check User Type
        return;
    }

    expressionCheck(expr->getValue());
    if (expr->getValue()->isError())
    {
        return;
    }

    if (!expr->getType())
    {
        expr->setType(expr->getValue()->getType());
    }

    if (!expr->getType()->isEqual(expr->getValue()->getType()))
    {
        expr->setError(Errors::getInstance().datatypeDifferent);
        return onError(expr);
    }

    getDeclarations().push_back(expr);
}

void AnalysisSemantic::variableExpressionCheck(VariableExpressionHandle expr)
{
    LOG(INFO) << "Variable Expression Check";

    auto decl = findDeclaration(expr->getIdentifier());
    if (!decl)
    {
        expr->setError(Errors::getInstance().variableNotDefined.withToken(expr->getToken()));
        return onError(expr);
    }

    expr->setType(decl->getType());
}

void AnalysisSemantic::assignmentExpressionCheck(AssignmentExpressionHandle expr)
{
    LOG(INFO) << "Assignment Expression Check";

    auto lhs = expr->getLHS();
    auto rhs = expr->getRHS();

    expressionCheck(lhs);
    expressionCheck(rhs);

    if (!lhs->getType()->isEqual(rhs->getType()))
    {
        expr->setError(Errors::getInstance().datatypeDifferent.withToken(expr->getToken()));
        return onError(expr);
    }

    if (lhs->getIsContant())
    {
        expr->setError(Errors::getInstance().lhsNotAssignable.withToken(lhs->getToken()));
        return onError(expr);
    }

    lhs->setAccess(AccessID::Load);
    rhs->setAccess(AccessID::Allocation);
}