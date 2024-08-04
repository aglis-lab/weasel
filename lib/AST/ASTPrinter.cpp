#include <fmt/core.h>

#include <weasel/Printer/Printer.h>
#include <weasel/AST/AST.h>
#include <weasel/IR/Codegen.h>

using namespace weasel;

#define UNIMPLEMENTED LOG(INFO) << "Unimplemented Printer";

void Function::printAsOperand(Printer *printer)
{
    UNIMPLEMENTED;
}

void LambdaStatement::printAsOperand(Printer *printer)
{
    UNIMPLEMENTED;
}

void ArgumentExpression::print(Printer *printer)
{
    UNIMPLEMENTED;
}

void ArgumentExpression::printAsOperand(Printer *printer)
{
    printer->print(this);
}

// GlobalVariable
void GlobalVariable::print(Printer *printer)
{
    printer->print(this);
}

void GlobalVariable::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// MethodCallExpression
void MethodCallExpression::print(Printer *printer)
{
    printer->print(this);
}

void MethodCallExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// Literal
void NumberLiteralExpression::print(Printer *printer)
{
    printer->print(this);
}

// DoubleLiteralExpression
void DoubleLiteralExpression::print(Printer *printer)
{
    fmt::print("Unimplemented double literal printer");
}

// FloatLiteralExpression
void FloatLiteralExpression::print(Printer *printer)
{
    fmt::print("Unimplemented float literal printer");
}

void FloatLiteralExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// BoolLiteralExpression
void BoolLiteralExpression::print(Printer *printer)
{
    printer->print(this);
}

// CharLiteralExpression
void CharLiteralExpression::print(Printer *printer)
{
    fmt::print("Unimplemented char printer");
}

// ArrayLiteralExpression
void ArrayLiteralExpression::print(Printer *printer)
{
    printer->print(this);
}

// StringLiteralExpression
void StringLiteralExpression::print(Printer *printer)
{
    printer->print(this);
}

// NilLiteralExpression
void NilLiteralExpression::print(Printer *printer)
{
    printer->printAsOperand(this);
}

// Statement
void DeclarationStatement::print(Printer *printer)
{
    printer->print(this);
}

// Expression
void VariableExpression::print(Printer *printer)
{
    printer->print(this);
}

// FieldExpression
void FieldExpression::print(Printer *printer)
{
    printer->print(this);
}

// ArrayExpression
void ArrayExpression::print(Printer *printer)
{
    printer->print(this);
}

// LogicalExpression
void LogicalExpression::print(Printer *printer)
{
    fmt::print("Unimplemented logical printer");
}

// ArithmeticExpression
void ArithmeticExpression::print(Printer *printer)
{
    printer->print(this);
}

// AssignmentExpression
void AssignmentExpression::print(Printer *printer)
{
    printer->print(this);
}

// ComparisonExpression
void ComparisonExpression::print(Printer *printer)
{
    printer->print(this);
}

// CallExpression
void CallExpression::print(Printer *printer)
{
    printer->print(this);
}

void CallExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

void LambdaStatement::print(Printer *printer)
{
    UNIMPLEMENTED;
}

// ReturnExpression
void ReturnExpression::print(Printer *printer)
{
    printer->print(this);
}

// StructExpression
void StructExpression::print(Printer *printer)
{
    printer->print(this);
}

void StructExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// BreakExpression
void BreakExpression::print(Printer *printer)
{
    printer->print(this);
}

void BreakExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// ContinueExpression
void ContinueExpression::print(Printer *printer)
{
    fmt::print("Unimplemented continue printer");
}

// CompoundStatement
void CompoundStatement::print(Printer *printer)
{
    printer->print(this);
}

// ConditionStatement
void ConditionStatement::print(Printer *printer)
{
    printer->print(this);
}

// LoopingStatement
void LoopingStatement::print(Printer *printer)
{
    printer->print(this);
}

// Operator Expression
void UnaryExpression::print(Printer *printer)
{
    printer->print(this);
}

// TypeCastExpression
void TypeCastExpression::print(Printer *printer)
{
    UNIMPLEMENTED;
}

// Funtion
void Function::print(Printer *printer)
{
    printer->print(this);
}

// Literal
void NumberLiteralExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// DoubleLiteralExpression
void DoubleLiteralExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// BoolLiteralExpression
void BoolLiteralExpression::printAsOperand(Printer *printer)
{
    printer->print(this);
}

// CharLiteralExpression
void CharLiteralExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// ArrayLiteralExpression
void ArrayLiteralExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// StringLiteralExpression
void StringLiteralExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// NilLiteralExpression
void NilLiteralExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// Statement
void DeclarationStatement::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// Expression
void VariableExpression::printAsOperand(Printer *printer)
{
    printer->print(this);
}

// FieldExpression
void FieldExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// ArrayExpression
void ArrayExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// LogicalExpression
void LogicalExpression::printAsOperand(Printer *printer)
{
    fmt::print("Unimplemented logical printer");
}

// ArithmeticExpression
void ArithmeticExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// AssignmentExpression
void AssignmentExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// ComparisonExpression
void ComparisonExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// ReturnExpression
void ReturnExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// ContinueExpression
void ContinueExpression::printAsOperand(Printer *printer)
{
    fmt::print("Unimplemented continue printer");
}

// CompoundStatement
void CompoundStatement::printAsOperand(Printer *printer)
{
    fmt::print("Unimplemented compound printer");
}

// ConditionStatement
void ConditionStatement::printAsOperand(Printer *printer)
{
    fmt::print("Unimplemented condition printer");
}

// LoopingStatement
void LoopingStatement::printAsOperand(Printer *printer)
{
    fmt::print("Unimplemented looping printer");
}

// Operator Expression
void UnaryExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// TypeCastExpression
void TypeCastExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}
