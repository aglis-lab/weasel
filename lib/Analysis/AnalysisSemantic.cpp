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
        if (item->getBody())
        {
            expressionCheck(item->getBody());
        }
    }
}

void AnalysisSemantic::userTypeCheck(StructTypeHandle expr)
{
}

void AnalysisSemantic::expressionCheck(ExpressionHandle expr)
{
    if (!expr)
    {
        LOG(ERROR) << Errors::getInstance().missingImplementation.getMessage();
        exit(1);
    }

    if (expr->isError())
    {
        return onError(expr);
    }

    auto exprName = typeid(*expr).name();
    if (typeid(CompoundStatement).name() == exprName)
    {
        return compoundStatementCheck(static_pointer_cast<CompoundStatement>(expr));
    }

    if (typeid(CallExpression).name() == exprName)
    {
        return callExpressionCheck(static_pointer_cast<CallExpression>(expr));
    }

    if (typeid(ConditionStatement).name() == exprName)
    {
        return conditionStatementChech(static_pointer_cast<ConditionStatement>(expr));
    }

    if (typeid(DeclarationStatement).name() == exprName)
    {
        return declarationStatementCheck(static_pointer_cast<DeclarationStatement>(expr));
    }

    if (typeid(VariableExpression).name() == exprName)
    {
        return variableExpressionCheck(static_pointer_cast<VariableExpression>(expr));
    }

    if (typeid(AssignmentExpression).name() == exprName)
    {
        return assignmentExpressionCheck(static_pointer_cast<AssignmentExpression>(expr));
    }

    if (typeid(ComparisonExpression).name() == exprName)
    {
        return comparisonExpressionCheck(static_pointer_cast<ComparisonExpression>(expr));
    }

    if (typeid(ReturnExpression).name() == exprName)
    {
        return returnExpressionCheck(static_pointer_cast<ReturnExpression>(expr));
    }

    if (typeid(LoopingStatement).name() == exprName)
    {
        return loopingStatementCheck(static_pointer_cast<LoopingStatement>(expr));
    }

    if (typeid(ArithmeticExpression).name() == exprName)
    {
        return arithmeticExpressionCheck(static_pointer_cast<ArithmeticExpression>(expr));
    }

    if (typeid(BreakExpression).name() == exprName)
    {
        return breakExpressionCheck(static_pointer_cast<BreakExpression>(expr));
    }

    if (typeid(ContinueExpression).name() == exprName)
    {
        return continueExpressionCheck(static_pointer_cast<ContinueExpression>(expr));
    }

    if (typeid(UnaryExpression).name() == exprName)
    {
        return unaryExpressionCheck(static_pointer_cast<UnaryExpression>(expr));
    }

    if (dynamic_pointer_cast<LiteralExpression>(expr))
    {
        LOG(INFO) << "Literal Expression Check";
        return;
    }

    LOG(ERROR) << expr->getToken().getEscapeValue() << " " << Errors::getInstance().missingImplementation.getMessage();
    exit(1);
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
        if (item->isError())
        {
            break;
        }
    }

    getDeclarations().resize(lastDeclaration);
}

void AnalysisSemantic::conditionStatementChech(ConditionStatementHandle expr)
{
    LOG(INFO) << "Condition Statement Check";

    for (auto item : expr->getConditions())
    {
        expressionCheck(item);
        if (item->isError())
        {
            break;
        }
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
        expr->setError(Errors::getInstance().datatypeDifferent.withToken(rhs->getToken()));
        return onError(expr);
    }

    if (lhs->getIsContant())
    {
        expr->setError(Errors::getInstance().lhsNotAssignable.withToken(lhs->getToken()));
        return onError(expr);
    }

    rhs->setAccess(AccessID::Allocation);
}

void AnalysisSemantic::comparisonExpressionCheck(ComparisonExpressionHandle expr)
{
    LOG(INFO) << "Comparison Expression Check";

    expressionCheck(expr->getLHS());
    expressionCheck(expr->getRHS());

    expr->setAccess(AccessID::Load);
    expr->setAccess(AccessID::Load);
}

void AnalysisSemantic::returnExpressionCheck(ReturnExpressionHandle expr)
{
    LOG(INFO) << "Return Expression Check";

    if (expr->getValue() && !expr->getValue()->isError())
    {
        expressionCheck(expr->getValue());

        if (expr->getValue()->isError())
        {
            return;
        }

        expr->setType(expr->getValue()->getType());
    }
}

void AnalysisSemantic::breakExpressionCheck(BreakExpressionHandle expr)
{
    LOG(INFO) << "Break Expression Check";

    auto val = expr->getValue();
    if (val)
    {
        expressionCheck(val);
        if (val->isError())
        {
            return;
        }

        if (!val->getType()->isBoolType())
        {
            expr->setError(Errors::getInstance().breakExpressionInvalid.withToken(val->getToken()));
            return onError(expr);
        }

        expr->setType(expr->getValue()->getType());
    }
}

void AnalysisSemantic::loopingStatementCheck(LoopingStatementHandle expr)
{
    LOG(INFO) << "Looping Statement Check";

    for (auto item : expr->getConditions())
    {
        expressionCheck(item);
    }

    expressionCheck(expr->getBody());
}

void AnalysisSemantic::continueExpressionCheck(ContinueExpressionHandle expr)
{
    LOG(INFO) << "Continue Expression Check";

    if (expr->getValue())
    {
        expressionCheck(expr->getValue());
    }
}

void AnalysisSemantic::arithmeticExpressionCheck(ArithmeticExpressionHandle expr)
{
    LOG(INFO) << "Arithmetic Expression Check";

    auto lhs = expr->getLHS();
    auto rhs = expr->getRHS();

    expressionCheck(lhs);
    expressionCheck(rhs);

    if (lhs->isError() || rhs->isError())
    {
        return;
    }

    if (!lhs->getType()->isEqual(rhs->getType()))
    {
        expr->setError(Errors::getInstance().datatypeDifferent.withToken(rhs->getToken()));
        return onError(expr);
    }

    expr->setType(lhs->getType());
}

void AnalysisSemantic::unaryExpressionCheck(UnaryExpressionHandle expr)
{
    LOG(INFO) << "Unary Expression Check";

    expressionCheck(expr->getExpression());
}