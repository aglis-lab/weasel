#include <cassert>
#include <unordered_set>

#include <weasel/Analysis/AnalysisSemantic.h>

void AnalysisSemantic::semanticCheck()
{
    // Struct Type
    for (auto item : getModule()->getUserTypes())
    {
        if (item->isError())
        {
            onStructError(item.get());
            break;
        }

        semantic(item.get());
    }

    // Functions
    for (auto item : getModule()->getFunctions())
    {
        if (item->isError())
        {
            onError(item.get());
            break;
        }

        for (auto &item : item->getArguments())
        {
            item->semantic(this);
        }

        semantic(item.get());
    }
}

void AnalysisSemantic::semantic(StructType *expr)
{
    // Check Duplicate Field
    unordered_set<string> checkName;

    for (auto &item : expr->getFields())
    {
        if (checkName.count(item.getIdentifier()) > 0)
        {
            expr->setError(Errors::getInstance().duplicateField.withToken(item.getToken()));
            return onStructError(expr);
        }

        checkName.insert(item.getIdentifier());

        if (item.getType()->isUnknownType())
        {
            auto structType = getModule()->findStructType(item.getIdentifier());
            if (!structType)
            {
                expr->setError(Errors::getInstance().userTypeNotDefined.withToken(item.getType()->getToken()));
                return onStructError(expr);
            }

            item.setType(structType);
        }

        item.getType()->semantic(this);
    }
}

void AnalysisSemantic::semantic(ArgumentExpression *expr)
{
    expr->getType()->semantic(this);
}

void AnalysisSemantic::semantic(Type *expr)
{
}

void AnalysisSemantic::semantic(Function *fun)
{
    auto lastDeclaration = getDeclarations().size();
    fun->getType()->semantic(this);

    // Check Arguments
    for (auto arg : fun->getArguments())
    {
        arg->semantic(this);

        getDeclarations().push_back(arg.get());
    }

    // Check Compound Statement
    if (fun->getBody())
    {
        semantic(fun->getBody().get());
    }

    getDeclarations().resize(lastDeclaration);
}

void AnalysisSemantic::semantic(CompoundStatement *expr)
{
    LOG(INFO) << "Compound Statement Check";

    if (!expr || expr->getBody().empty())
    {
        return;
    }

    auto lastDeclaration = getDeclarations().size();
    for (auto item : expr->getBody())
    {
        item->semantic(this);
        if (item->isError())
        {
            break;
        }
    }

    getDeclarations().resize(lastDeclaration);
}

void AnalysisSemantic::semantic(ConditionStatement *expr)
{
    LOG(INFO) << "Condition Statement Check";

    for (auto item : expr->getConditions())
    {
        item->semantic(this);
        if (item->isError())
        {
            break;
        }
    }

    for (auto item : expr->getStatements())
    {
        item->semantic(this);
    }
}

void AnalysisSemantic::semantic(CallExpression *expr)
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
        item->semantic(this);
    }

    // auto count = fun->isVararg() ? fun->getArguments().size() - 1 : fun->getArguments().size();
    // for (size_t i = 0; i < count; i++)
    // {
    //     auto exprArg = expr->getArguments()[i];
    //     auto funArg = fun->getArguments()[i];

    //     exprArg->semantic(this);
    //     // TODO: Check Argument Type
    //     // if (!exprArg->getType()->isEqual(funArg->getType()))
    //     // {
    //     //     expr->setError(Errors::getInstance().datatypeDifferent.withToken(exprArg->getToken()));
    //     //     return onError(expr);
    //     // }
    // }
}

void AnalysisSemantic::semantic(DeclarationStatement *expr)
{
    LOG(INFO) << "Declaration Statement Check";

    if (expr->getType() && expr->getType()->isUnknownType())
    {
        auto newType = getModule()->findStructType(expr->getType()->getToken().getValue());
        if (!newType)
        {
            expr->setError(Errors::getInstance().userTypeNotDefined.withToken(expr->getType()->getToken()));
            return onError(expr);
        }

        expr->setType(newType);
    }

    if (!expr->getValue())
    {
        getDeclarations().push_back(expr);
        return;
    }

    expr->getValue()->semantic(this);
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
        expr->setError(Errors::getInstance().datatypeDifferent.withToken(expr->getValue()->getToken()));
        return onError(expr);
    }

    getDeclarations().push_back(expr);
}

void AnalysisSemantic::semantic(VariableExpression *expr)
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

void AnalysisSemantic::semantic(AssignmentExpression *expr)
{
    LOG(INFO) << "Assignment Expression Check";

    auto lhs = expr->getLHS();
    auto rhs = expr->getRHS();

    lhs->semantic(this);
    rhs->semantic(this);

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

void AnalysisSemantic::semantic(ComparisonExpression *expr)
{
    LOG(INFO) << "Comparison Expression Check";

    expr->getLHS()->semantic(this);
    expr->getRHS()->semantic(this);

    expr->setAccess(AccessID::Load);
    expr->setAccess(AccessID::Load);
}

void AnalysisSemantic::semantic(ReturnExpression *expr)
{
    LOG(INFO) << "Return Expression Check";

    if (expr->getValue() && !expr->getValue()->isError())
    {
        expr->getValue()->semantic(this);

        if (expr->getValue()->isError())
        {
            return;
        }

        expr->setType(expr->getValue()->getType());
    }
}

void AnalysisSemantic::semantic(BreakExpression *expr)
{
    LOG(INFO) << "Break Expression Check";

    auto val = expr->getValue();
    if (val)
    {
        val->semantic(this);
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

void AnalysisSemantic::semantic(LoopingStatement *expr)
{
    LOG(INFO) << "Looping Statement Check";

    for (auto item : expr->getConditions())
    {
        item->semantic(this);
    }

    expr->getBody()->semantic(this);
}

void AnalysisSemantic::semantic(ContinueExpression *expr)
{
    LOG(INFO) << "Continue Expression Check";

    if (expr->getValue())
    {
        expr->getValue()->semantic(this);
    }
}

void AnalysisSemantic::semantic(ArithmeticExpression *expr)
{
    LOG(INFO) << "Arithmetic Expression Check";

    auto lhs = expr->getLHS();
    auto rhs = expr->getRHS();

    lhs->semantic(this);
    rhs->semantic(this);

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

void AnalysisSemantic::semantic(UnaryExpression *expr)
{
    LOG(INFO) << "Unary Expression Check";

    expr->getExpression()->semantic(this);
    if (expr->getOperator() == UnaryExpression::Borrow)
    {
        expr->setType(Type::getReferenceType(expr->getExpression()->getType()));
    }
    else if (expr->getOperator() == UnaryExpression::Dereference)
    {
        expr->setType(expr->getExpression()->getType());
    }
}

void AnalysisSemantic::semantic(StructExpression *expr)
{
    LOG(INFO) << "Struct Expression Check";

    auto newType = getModule()->findStructType(expr->getIdentifier());
    if (!newType)
    {
        expr->setError(Errors::getInstance().userTypeNotDefined.withToken(expr->getToken()));
        return onError(expr);
    }

    expr->setType(newType);
    for (auto item : expr->getFields())
    {
        item->getValue()->semantic(this);
    }
}

void AnalysisSemantic::semantic(FieldExpression *expr)
{
    LOG(INFO) << "Field Expression Check";

    expr->getParentField()->semantic(this);
    if (expr->getParentField()->isError())
    {
        return;
    }

    auto parentType = expr->getParentField()->getType();
    if (!parentType->isStructType())
    {
        expr->setError(Errors::getInstance().shouldStructType.withToken(expr->getParentField()->getToken()));
        return onError(expr);
    }

    assert(typeid(StructType) == typeid(*parentType) && "failed to cast type into struct type");

    auto structType = static_pointer_cast<StructType>(parentType);
    auto [idx, field] = structType->findTypeName(expr->getIdentifier());
    if (idx == -1)
    {
        expr->setError(Errors::getInstance().fieldNotExist.withToken(expr->getToken()));
        return onError(expr);
    }

    expr->setType(field->getType());
}

void AnalysisSemantic::semantic(TypeCastExpression *expr)
{
    LOG(INFO) << "Type Cast Expression Check";

    expr->getValue()->semantic(this);
    expr->getType()->semantic(this);
}
