#include <weasel/AST/AST.h>
#include <weasel/Analysis/AnalysisSemantic.h>

using namespace weasel;

#define UNIMPLEMENTED LOG(INFO) << "Unimplemented Codegen";

void ArgumentExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void GlobalVariable::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void MethodCallExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void NumberLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void DoubleLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void FloatLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void BoolLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void CharLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void ArrayLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void StringLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void NilLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void DeclarationStatement::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void VariableExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void FieldExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void ArrayExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void ArithmeticExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void LogicalExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void AssignmentExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void ComparisonExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void CallExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void ReturnExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void BreakExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void ContinueExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void StructExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void Function::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void UnaryExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void TypeCastExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED;
}

void CompoundStatement::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void ConditionStatement::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void LoopingStatement::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}
