#include <cassert>
#include <unordered_set>

#include <weasel/Analysis/AnalysisSemantic.h>

#define SEMANTIC(X) LOG(INFO) << "Semantic Check " << X

void AnalysisSemantic::semanticCheck()
{
    // Global Variable
    for (auto &item : getModule()->getGlobalVariables())
    {
        semantic(item.get());
    }

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
    for (auto fun : getModule()->getFunctions())
    {
        if (fun->isError())
        {
            onError(fun.get());
            break;
        }

        // Check Function Type
        for (auto &item : fun->getArguments())
        {
            if (item->getType()->isUnknownType())
            {
                item->setType(unknownType(item->getType()));
            }
            else
            {
                item->accept(this);
            }
        }

        // Check Function Body
        semantic(fun.get());
    }
}

void AnalysisSemantic::semantic(ArgumentExpression *expr)
{
    SEMANTIC("ArgumentExpression");

    expr->getType()->accept(this);
}

void AnalysisSemantic::semantic(FunctionType *expr)
{
    SEMANTIC("FunctionType");

    if (expr->getReturnType()->isUnknownType())
    {
        expr->setReturnType(unknownType(expr->getReturnType()));
    }
}

void AnalysisSemantic::semantic(StructType *expr)
{
    SEMANTIC("StructType") << " " << expr->getTypeName();

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

        item.getType()->accept(this);
    }
}

void AnalysisSemantic::semantic(Type *expr)
{
    SEMANTIC("Type") << " " << expr->getTypeName();

    if ((expr->isPointerType() || expr->isReferenceType()) && expr->getContainedType()->isUnknownType())
    {
        auto structType = getModule()->findStructType(expr->getContainedType()->getToken().getValue());

        // TODO: Create Better Error Handling with more generic and understanable error
        assert(structType && "underlying type should be exist");

        expr->setContainedType(structType);
    }

    if (expr->isArrayType())
    {
        expr->getContainedType()->accept(this);
    }
}

TypeHandle AnalysisSemantic::unknownType(TypeHandle expr)
{
    SEMANTIC("UnknownType Expression");

    if (!expr->isUnknownType())
    {
        return expr;
    }

    auto structType = getModule()->findStructType(expr->getToken().getValue());
    if (!structType)
    {
        expr->setError(Errors::getInstance().userTypeNotDefined.withToken(expr->getToken()));
        return expr;
    }

    return structType;
}

void AnalysisSemantic::semantic(Function *fun)
{
    SEMANTIC("Function") << " " << fun->getIdentifier();

    auto lastDeclaration = getDeclarations().size();
    auto type = make_shared<FunctionType>();

    type->setReturnType(fun->getType());
    type->setIsVararg(fun->isVararg());

    // Check Arguments
    for (auto arg : fun->getArguments())
    {
        type->getArguments().push_back(arg->getType());

        getDeclarations().push_back(arg.get());
    }

    // Function Type
    type->accept(this);
    fun->setType(type);

    // Check Compound Statement
    setCurrentFunction(fun);
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
        item->accept(this);
        if (item->isError())
        {
            break;
        }
    }

    getDeclarations().resize(lastDeclaration);
}

void AnalysisSemantic::semantic(ConditionStatement *expr)
{
    SEMANTIC("ConditionStatement");

    for (auto item : expr->getConditions())
    {
        item->accept(this);
        if (item->isError())
        {
            break;
        }
    }

    for (auto item : expr->getStatements())
    {
        item->accept(this);
    }
}

// TODO: Add Function Type for lambda and Capture all value needed
void AnalysisSemantic::semantic(CallExpression *expr)
{
    SEMANTIC("CallExpression");

    expr->getLHS()->accept(this);
    if (expr->getLHS()->isError() || !expr->getLHS()->getType()->isFunctionType())
    {
        expr->setError(Errors::getInstance().datatypeDifferent.withToken(expr->getLHS()->getToken()));
        return onError(expr);
    }

    auto funType = static_pointer_cast<FunctionType>(expr->getLHS()->getType());
    assert(funType);

    expr->setType(funType->getReturnType());
    if ((!funType->isVararg() && expr->getArguments().size() != funType->getArguments().size()) ||
        (funType->isVararg() && expr->getArguments().size() < funType->getArguments().size() - 1))
    {
        expr->setError(Errors::getInstance().invalidArgumentSize.withToken(expr->getLHS()->getToken()));
        return onError(expr);
    }

    auto argIndex = 0;
    for (auto &item : expr->getArguments())
    {
        auto argType = funType->getArguments()[argIndex];

        item->accept(this);
        if (item->isError())
        {
            return;
        }

        if (item->isNilExpression() && argType->isPointerType())
        {
            continue;
        }

        assert(item->getType());
        if (!argType->isEqual(item->getType()))
        {
            item->setError(Errors::getInstance().datatypeDifferent.withToken(item->getToken()));
            return onError(item.get());
        }

        argIndex++;
    }

    assert(expr->getType() && "call expression should be have a type");
}

void AnalysisSemantic::semantic(DeclarationStatement *expr)
{
    SEMANTIC("DeclarationStatement");

    if (expr->getType())
    {
        if (expr->getType()->isUnknownType())
        {
            expr->setType(unknownType(expr->getType()));
        }

        expr->getType()->accept(this);
    }

    if (!expr->getValue())
    {
        assert(expr->getType());

        // Default value for pointer
        if (expr->getType()->isPointerType())
        {
            expr->setValue(make_shared<NilLiteralExpression>(Token::create()));
            expr->getValue()->getType()->setContainedType(expr->getType()->getContainedType());
        }

        // Default value for Integer
        if (expr->getType()->isIntegerType())
        {
            expr->setValue(make_shared<NumberLiteralExpression>(Token::create(), 0, expr->getType()->getTypeWidth()));
        }

        // Default Value for float
        if (expr->getType()->isFloatType())
        {
            expr->setValue(make_shared<FloatLiteralExpression>(Token::create(), 0));
        }

        // Default Value for Double
        if (expr->getType()->isDoubleType())
        {
            expr->setValue(make_shared<DoubleLiteralExpression>(Token::create(), 0));
        }

        getDeclarations().push_back(expr);
        return;
    }

    expr->getValue()->accept(this);
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
    SEMANTIC("VariableExpression") << " " << expr->getIdentifier();

    Expression *decl = findDeclaration(expr->getIdentifier());
    if (!decl)
    {
        auto fun = getModule()->findFunction(expr->getIdentifier());
        if (!fun)
        {
            expr->setError(Errors::getInstance().variableNotDefined.withToken(expr->getToken()));
            return onError(expr);
        }

        decl = fun.get();
    }

    assert(decl);
    assert(decl->getType());

    expr->setType(decl->getType());
    expr->setDeclarationValue(decl);
}

void AnalysisSemantic::semantic(AssignmentExpression *expr)
{
    SEMANTIC("AssignmentExpression");

    auto lhs = expr->getLHS();
    auto rhs = expr->getRHS();

    lhs->accept(this);
    if (lhs->isError())
    {
        return;
    }

    rhs->accept(this);
    if (rhs->isError())
    {
        return;
    }

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
    expr->setType(lhs->getType());
}

void AnalysisSemantic::semantic(ComparisonExpression *expr)
{
    LOG(INFO) << "Comparison Expression Check";

    expr->getLHS()->accept(this);
    expr->getRHS()->accept(this);

    expr->setAccess(AccessID::Load);
    expr->setAccess(AccessID::Load);
}

void AnalysisSemantic::semantic(ReturnExpression *expr)
{
    LOG(INFO) << "Return Expression Check";

    if (expr->getValue() && !expr->getValue()->isError())
    {
        expr->getValue()->accept(this);

        if (expr->getValue()->isError())
        {
            return;
        }

        expr->setType(expr->getValue()->getType());
    }

    if (!expr->getType())
    {
        expr->setType(Type::getVoidType());
    }

    if (!expr->getType()->isEqual(getCurrentFunction()->getReturnType()))
    {
        expr->setError(Errors::getInstance().datatypeDifferent.withToken(expr->getType()->getToken()));
        return onError(expr);
    }
}

void AnalysisSemantic::semantic(BreakExpression *expr)
{
    LOG(INFO) << "Break Expression Check";

    auto val = expr->getValue();
    if (val)
    {
        val->accept(this);
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
        item->accept(this);
    }

    expr->getBody()->accept(this);
}

void AnalysisSemantic::semantic(ContinueExpression *expr)
{
    LOG(INFO) << "Continue Expression Check";

    if (expr->getValue())
    {
        expr->getValue()->accept(this);
    }
}

void AnalysisSemantic::semantic(ArithmeticExpression *expr)
{
    LOG(INFO) << "Arithmetic Expression Check";

    auto lhs = expr->getLHS();
    auto rhs = expr->getRHS();

    lhs->accept(this);
    rhs->accept(this);

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

    expr->setAccess(expr->getValue()->getAccess());
    expr->getValue()->accept(this);

    switch (expr->getOperator())
    {
    case UnaryExpression::Borrow:
    {
        expr->setType(Type::getReferenceType(expr->getValue()->getType()));
        break;
    }
    case UnaryExpression::Dereference:
    {
        if (!expr->getValue()->getType()->isPointerType() && !expr->getValue()->getType()->isReferenceType())
        {
            expr->setError(Errors::getInstance().failedDereference.withToken(expr->getValue()->getToken()));
            return onError(expr);
        }

        expr->setType(expr->getValue()->getType()->getContainedType());
        break;
    }
    case UnaryExpression::Not:
    {
        if (!expr->getValue()->getType()->isBoolType())
        {
            expr->setError(Errors::getInstance().expectedBoolType.withToken(expr->getValue()->getToken()));
            return onError(expr);
        }

        expr->setType(expr->getValue()->getType());
        break;
    }
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
        item->getValue()->accept(this);
    }
}

void AnalysisSemantic::semantic(FieldExpression *expr)
{
    SEMANTIC("FieldExpression");

    expr->getLHS()->accept(this);
    if (expr->getLHS()->isError())
    {
        return;
    }

    // Check if parent field is a pointer or reference
    // Let just dereference it
    if (expr->getLHS()->getType()->isPointerType() || expr->getLHS()->getType()->isReferenceType())
    {
        expr->setLHS(make_shared<UnaryExpression>(Token::create(), UnaryExpression::Dereference, expr->getLHS()));
        expr->getLHS()->accept(this);
    }

    assert(!expr->isError() && "expression should be valid code");

    auto parentType = expr->getLHS()->getType();
    if (!parentType->isStructType())
    {
        expr->setError(Errors::getInstance().shouldStructType.withToken(expr->getLHS()->getToken()));
        return onError(expr);
    }

    assert(typeid(StructType) == typeid(*parentType) && "failed to cast type into struct type");

    auto structType = static_pointer_cast<StructType>(parentType);
    auto [idx, field] = structType->findTypeName(expr->getField());
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

    expr->getValue()->accept(this);
    expr->getValue()->getType()->accept(this);
    expr->getType()->accept(this);
}

void AnalysisSemantic::semantic(NilLiteralExpression *expr)
{
    SEMANTIC("NilLiteralExpression");

    expr->getType()->accept(this);
}

void AnalysisSemantic::semantic(IndexExpression *expr)
{
    SEMANTIC("IndexExpression");

    assert(expr->getLHS());
    expr->getLHS()->accept(this);

    assert(expr->getIndex());
    expr->getIndex()->accept(this);

    expr->setType(expr->getLHS()->getType()->getContainedType());
    assert(expr->getType());
}

void AnalysisSemantic::semantic(ArrayExpression *expr)
{
    SEMANTIC("ArrayExpression");
}
