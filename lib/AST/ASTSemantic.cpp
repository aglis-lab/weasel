#include <weasel/AST/AST.h>
#include <weasel/Analysis/AnalysisSemantic.h>

using namespace weasel;

#define UNIMPLEMENTED(XX) LOG(INFO) << "Unimplemented Analysis Semantic " << XX;

void ArgumentExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void GlobalVariable::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("GlobalVariable");
}

void MethodCallExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("MethodCallExpression");
}

void NumberLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("NumberLiteralExpression");
}

void DoubleLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("DoubleLiteralExpression");
}

void FloatLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("FloatLiteralExpression");
}

void BoolLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("BoolLiteralExpression");
}

void CharLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("CharLiteralExpression");
}

void ArrayLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("ArrayLiteralExpression");
}

void StringLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("StringLiteralExpression");
}

void NilLiteralExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
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
    UNIMPLEMENTED("ArrayExpression");
}

void ArithmeticExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void LogicalExpression::semantic(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("LogicalExpression");
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
    UNIMPLEMENTED("Function");
}

void UnaryExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void TypeCastExpression::semantic(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
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
