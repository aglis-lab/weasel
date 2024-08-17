#include <weasel/AST/AST.h>
#include <weasel/Analysis/AnalysisEvaluate.h>

using namespace weasel;

#define UNIMPLEMENTED LOG(INFO) << "UNIMPLEMENTED; Evaluate Analysis"
#define IMPLEMENT(XX) LOG(INFO) << "AnalysisEvaluate " << XX

EvaluationValue LambdaStatement::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue ArgumentExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue GlobalVariable::accept(AnalysisEvaluate *c)
{
    IMPLEMENT("AnalysisEvaluate");

    if (isConstant() && getType()->isPrimitiveType())
    {
        return getValue()->accept(c);
    }

    return nullopt;
}

EvaluationValue MethodCallExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue NumberLiteralExpression::accept(AnalysisEvaluate *c)
{
    return getValue();
}

EvaluationValue DoubleLiteralExpression::accept(AnalysisEvaluate *c)
{
    return getValue();
}

EvaluationValue FloatLiteralExpression::accept(AnalysisEvaluate *c)
{
    return getValue();
}

EvaluationValue BoolLiteralExpression::accept(AnalysisEvaluate *c)
{
    return getValue();
}

EvaluationValue CharLiteralExpression::accept(AnalysisEvaluate *c)
{
    return getValue();
}

EvaluationValue ArrayExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue StringLiteralExpression::accept(AnalysisEvaluate *c)
{
    return getValue();
}

EvaluationValue NilLiteralExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue DeclarationStatement::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue VariableExpression::accept(AnalysisEvaluate *c)
{
    assert(getDeclarationValue());

    return getDeclarationValue()->accept(c);
}

EvaluationValue FieldExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue IndexExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue ArithmeticExpression::accept(AnalysisEvaluate *c)
{
    if (!getRHS()->getType()->isEqual(getLHS()->getType()))
    {
        return nullopt;
    }

    auto rhsV = getRHS()->accept(c);
    if (!rhsV)
    {
        return nullopt;
    }

    auto lhsV = getLHS()->accept(c);
    if (!lhsV)
    {
        return nullopt;
    }

    if (getRHS()->getType()->isIntegerType())
    {
        return get<long>(rhsV.value()) + get<long>(lhsV.value());
    }

    if (getRHS()->getType()->isDoubleFloatType())
    {
        return get<double>(rhsV.value()) + get<double>(lhsV.value());
    }

    if (getRHS()->getType()->isArrayType())
    {
        return get<string>(rhsV.value()) + get<string>(lhsV.value());
    }

    return nullopt;
}

EvaluationValue LogicalExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue AssignmentExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue ComparisonExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue CallExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue ReturnExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue BreakExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue ContinueExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue StructExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue Function::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue UnaryExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue TypeCastExpression::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue CompoundStatement::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue ConditionStatement::accept(AnalysisEvaluate *c)
{
    return nullopt;
}

EvaluationValue LoopingStatement::accept(AnalysisEvaluate *c)
{
    return nullopt;
}
