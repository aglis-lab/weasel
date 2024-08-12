#include <weasel/AST/AST.h>
#include <weasel/Analysis/AnalysisSemantic.h>

using namespace weasel;

#define UNIMPLEMENTED(XX) LOG(INFO) << "Unimplemented Analysis Semantic " << XX;

void ArgumentExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void LambdaStatement::accept(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("LambdaStatement");
}

void GlobalVariable::accept(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("GlobalVariable");
}

void MethodCallExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void NumberLiteralExpression::accept(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("NumberLiteralExpression");
}

void DoubleLiteralExpression::accept(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("DoubleLiteralExpression");
}

void FloatLiteralExpression::accept(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("FloatLiteralExpression");
}

void BoolLiteralExpression::accept(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("BoolLiteralExpression");
}

void CharLiteralExpression::accept(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("CharLiteralExpression");
}

void ArrayExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void StringLiteralExpression::accept(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("StringLiteralExpression");
}

void NilLiteralExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void DeclarationStatement::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void VariableExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void FieldExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void IndexExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void ArithmeticExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void LogicalExpression::accept(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("LogicalExpression");
}

void AssignmentExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void ComparisonExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void CallExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void ReturnExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void BreakExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void ContinueExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void StructExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void Function::accept(AnalysisSemantic *analysis)
{
    UNIMPLEMENTED("Function");
}

void UnaryExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void TypeCastExpression::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void CompoundStatement::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void ConditionStatement::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}

void LoopingStatement::accept(AnalysisSemantic *analysis)
{
    analysis->semantic(this);
}
