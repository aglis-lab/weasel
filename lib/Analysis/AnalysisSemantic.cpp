#include <cassert>
#include <unordered_set>

#include <weasel/Analysis/AnalysisSemantic.h>
#include <weasel/Basic/Defer.h>

#define SEMANTIC(X) LOG(INFO) << "Semantic Check " << X

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

        accept(item.get());
    }

    // Global Variable
    for (auto &item : getModule()->getGlobalVariables())
    {
        accept(item.get());
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
            if (item->isImplThis())
            {
                if (item->isImplThisReference())
                {
                    item->setType(Type::getPointerType(fun->getImplType()));
                }
                else
                {
                    item->setType(fun->getImplType());
                }
            }

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
        accept(fun.get());
    }
}

void AnalysisSemantic::accept(ArgumentExpression *expr)
{
    SEMANTIC("ArgumentExpression");

    expr->getType()->accept(this);
}

void AnalysisSemantic::accept(FunctionType *expr)
{
    SEMANTIC("FunctionType");

    if (expr->getReturnType()->isUnknownType())
    {
        expr->setReturnType(unknownType(expr->getReturnType()));
    }
}

void AnalysisSemantic::accept(StructType *expr)
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

void AnalysisSemantic::accept(Type *expr)
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
    SEMANTIC("UnknownType");

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

void AnalysisSemantic::accept(Function *fun)
{
    SEMANTIC("Function") << " " << fun->getIdentifier();

    saveState();
    defer { restoreState(); };

    auto type = make_shared<FunctionType>();
    if (fun->isImplTypeExist())
    {
        if (fun->getArguments().size() > 0 && fun->getArguments().front()->isImplThis())
        {
            fun->setImplType(unknownType(fun->getImplType()));
            type->setIsStatic(false);
        }
    }

    type->setReturnType(fun->getType());
    type->setIsVararg(fun->isVararg());

    // Check Impl this
    auto argSize = fun->getArguments().size();
    for (size_t i = 1; i < argSize; i++)
    {
        auto arg = fun->getArguments()[i];

        if (arg->isImplThis())
        {
            arg->setError(Errors::getInstance().unexpectedThisArgument.withToken(arg->getToken()));
            onError(arg.get());
        }
    }

    // Check Arguments
    for (auto arg : fun->getArguments())
    {
        if (arg->isError())
        {
            continue;
        }

        type->getArguments().push_back(arg->getType());

        addDeclaration(arg.get());
    }

    // Function Type
    type->accept(this);
    fun->setType(type);

    // Check Compound Statement
    setCurrentFunction(fun);

    if (fun->getBody())
    {
        accept(fun->getBody().get());
    }

    // Find Duplicate Function
    if (getModule()->findFunctions(fun->getIdentifier(), fun->getImplType(), type->getIstatic()).size() >= 2)
    {
        fun->setError(Errors::getInstance().duplicateFunction.withToken(fun->getToken()));
        return onError(fun);
    }
}

void AnalysisSemantic::accept(CompoundStatement *expr)
{
    LOG(INFO) << "Compound Statement Check";

    saveState();
    defer { restoreState(); };

    if (!expr || expr->getBody().empty())
    {
        return;
    }

    auto index = 1;
    for (auto item : expr->getBody())
    {
        item->accept(this);
        if (item->isError())
        {
            break;
        }
    }
}

void AnalysisSemantic::accept(ConditionStatement *expr)
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
void AnalysisSemantic::accept(CallExpression *expr)
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

    if (!funType->getIstatic())
    {
        auto methodCall = static_pointer_cast<MethodCallExpression>(expr->getLHS());
        assert(methodCall);

        auto callLHS = methodCall->getLHS();
        auto lhsType = callLHS->getType();
        auto thisType = funType->getArguments().front();
        if (!lhsType->isEqual(thisType))
        {
            // LHS is pointer
            if (lhsType->isPointerType() || lhsType->isReferenceType())
            {
                callLHS = make_shared<UnaryExpression>(Token::create(), UnaryExpression::Dereference, callLHS);
            }
            else
            {
                // LHS is value
                callLHS = make_shared<UnaryExpression>(Token::create(), UnaryExpression::Borrow, callLHS);
            }
        }

        expr->getArguments().insert(expr->getArguments().begin(), callLHS);
    }

    expr->setType(funType->getReturnType());
    auto argSize = funType->getArguments().size();
    if ((!funType->isVararg() && expr->getArguments().size() != argSize) ||
        (funType->isVararg() && expr->getArguments().size() < argSize - 1))
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

void AnalysisSemantic::accept(DeclarationStatement *expr)
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

        return addDeclaration(expr);
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

    addDeclaration(expr);
}

void AnalysisSemantic::accept(VariableExpression *expr)
{
    SEMANTIC("VariableExpression") << " " << expr->getIdentifier();

    Expression *decl = findDeclaration(expr->getIdentifier());
    if (!decl)
    {
        auto fun = getModule()->getFunction(expr->getIdentifier());
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

void AnalysisSemantic::accept(AssignmentExpression *expr)
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

void AnalysisSemantic::accept(ComparisonExpression *expr)
{
    LOG(INFO) << "Comparison Expression Check";

    expr->getLHS()->accept(this);
    expr->getRHS()->accept(this);

    expr->setAccess(AccessID::Load);
    expr->setAccess(AccessID::Load);
}

void AnalysisSemantic::accept(ReturnExpression *expr)
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

void AnalysisSemantic::accept(BreakExpression *expr)
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

void AnalysisSemantic::accept(LoopingStatement *expr)
{
    LOG(INFO) << "Looping Statement Check";

    for (auto item : expr->getConditions())
    {
        item->accept(this);
    }

    expr->getBody()->accept(this);
}

void AnalysisSemantic::accept(ContinueExpression *expr)
{
    LOG(INFO) << "Continue Expression Check";

    if (expr->getValue())
    {
        expr->getValue()->accept(this);
    }
}

void AnalysisSemantic::accept(ArithmeticExpression *expr)
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

void AnalysisSemantic::accept(UnaryExpression *expr)
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

void AnalysisSemantic::accept(StructExpression *expr)
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

void AnalysisSemantic::accept(FieldExpression *expr)
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

void AnalysisSemantic::accept(TypeCastExpression *expr)
{
    LOG(INFO) << "Type Cast Expression Check";

    expr->getValue()->accept(this);
    expr->getValue()->getType()->accept(this);
    expr->getType()->accept(this);
}

void AnalysisSemantic::accept(NilLiteralExpression *expr)
{
    SEMANTIC("NilLiteralExpression");

    expr->getType()->accept(this);
}

void AnalysisSemantic::accept(IndexExpression *expr)
{
    SEMANTIC("IndexExpression");

    assert(expr->getLHS());
    expr->getLHS()->accept(this);

    assert(expr->getIndex());
    expr->getIndex()->accept(this);

    expr->setType(expr->getLHS()->getType()->getContainedType());
    assert(expr->getType());
}

void AnalysisSemantic::accept(ArrayExpression *expr)
{
    SEMANTIC("ArrayExpression");

    for (auto &item : expr->getItems())
    {
        item->accept(this);
    }
}

void AnalysisSemantic::accept(MethodCallExpression *expr)
{
    SEMANTIC("MethodCallExpression") << " " << expr->getIdentifier();

    expr->getLHS()->accept(this);
    auto implType = expr->getLHS()->getType();
    if (implType->isReferenceType() || implType->isPointerType())
    {
        implType = implType->getContainedType();
    }

    auto fun = getModule()->getFunction(expr->getIdentifier(), implType, false);
    if (!fun)
    {
        expr->setError(Errors::getInstance().functionNotDefined.withToken(expr->getToken()));
        return onError(expr);
    }

    expr->setType(fun->getType());
    expr->setDeclarationValue(fun.get());
}
