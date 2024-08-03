#include <cassert>
#include <unordered_set>

#include <weasel/Analysis/AnalysisSemantic.h>

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
        fun->getType()->accept(this);
        for (auto &item : fun->getArguments())
        {
            unknownType(item.get());
            item->accept(this);
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
}

void AnalysisSemantic::semantic(StructType *expr)
{
    SEMANTIC("StructType");

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
}

void AnalysisSemantic::unknownType(Expression *expr)
{
    SEMANTIC("unknownType Expression");

    if (expr->getType()->isUnknownType())
    {
        auto structType = getModule()->findStructType(expr->getType()->getToken().getValue());
        if (!structType)
        {
            expr->setError(Errors::getInstance().userTypeNotDefined.withToken(expr->getType()->getToken()));
            return onError(expr);
        }

        expr->setType(structType);
    }
}

void AnalysisSemantic::semantic(Function *fun)
{
    SEMANTIC("Function") << " " << fun->getIdentifier();

    auto lastDeclaration = getDeclarations().size();
    auto type = make_shared<FunctionType>();

    fun->getType()->accept(this);
    type->setReturnType(fun->getType());

    // Check Arguments
    for (auto arg : fun->getArguments())
    {
        arg->accept(this);

        type->getArguments().push_back(arg->getType());

        getDeclarations().push_back(arg.get());
    }

    // Function Type
    fun->setType(type);

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

void AnalysisSemantic::semantic(CallExpression *expr)
{
    SEMANTIC("CallExpression");

    // Check Function Call
    auto fun = getModule()->findFunction(expr->getIdentifier());
    if (!fun)
    {
        expr->setError(Errors::getInstance().functionNotDefined.withToken(expr->getToken()));
        return onError(expr);
    }

    expr->setType(fun->getType());
    // expr->setFunction(fun);

    assert(expr->getType() && "call expression should be have a type");

    // Check Argument
    for (size_t i = 0; i < expr->getArguments().size(); i++)
    {
        auto arg = expr->getArguments()[i];
        auto funArg = fun->getArguments()[min(fun->getArguments().size() - 1, i)];

        arg->accept(this);
        if (arg->isError())
        {
            return;
        }

        if (!arg->getType()->isEqual(funArg->getType()))
        {
            expr->setError(Errors::getInstance().datatypeDifferent.withToken(arg->getToken()));
            return onError(expr);
        }
    }
}

void AnalysisSemantic::semantic(DeclarationStatement *expr)
{
    SEMANTIC("DeclarationStatement");

    if (expr->getType())
    {
        unknownType(expr);
        expr->getType()->accept(this);
    }

    if (!expr->getValue())
    {
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
    SEMANTIC("VariableExpression");

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
    if (expr->getOperator() == UnaryExpression::Borrow)
    {
        expr->setType(Type::getReferenceType(expr->getValue()->getType()));
    }
    else if (expr->getOperator() == UnaryExpression::Dereference)
    {
        if (!expr->getValue()->getType()->isPointerType() && !expr->getValue()->getType()->isReferenceType())
        {
            expr->setError(Errors::getInstance().failedDereference.withToken(expr->getValue()->getToken()));
            return onError(expr);
        }

        expr->setType(expr->getValue()->getType()->getContainedType());
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

    expr->getParentField()->accept(this);
    if (expr->getParentField()->isError())
    {
        return;
    }

    // Check if parent field is a pointer or reference
    // Let just dereference it
    if (expr->getParentField()->getType()->isPointerType() || expr->getParentField()->getType()->isReferenceType())
    {
        expr->setParentField(make_shared<UnaryExpression>(Token::create(), UnaryExpression::Dereference, expr->getParentField()));
        expr->getParentField()->accept(this);
    }

    assert(!expr->isError() && "expression should be valid code");

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

    expr->getValue()->accept(this);
    expr->getValue()->getType()->accept(this);
    expr->getType()->accept(this);
}

void AnalysisSemantic::semantic(NilLiteralExpression *expr)
{
    SEMANTIC("NilLiteralExpression");

    expr->getType()->accept(this);
}
