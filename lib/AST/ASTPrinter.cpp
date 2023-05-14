#include <fmt/core.h>

#include <weasel/Printer/Printer.h>
#include <weasel/AST/AST.h>
#include <weasel/IR/Codegen.h>

// Literal
void weasel::NumberLiteralExpression::print(Printer *printer)
{
    printer->print(this);
}

// DoubleLiteralExpression
void weasel::DoubleLiteralExpression::print(Printer *printer)
{
    fmt::print("Unimplemented double literal printer");
}

// FloatLiteralExpression
void weasel::FloatLiteralExpression::print(Printer *printer)
{
    fmt::print("Unimplemented float literal printer");
}

// BoolLiteralExpression
void weasel::BoolLiteralExpression::print(Printer *printer)
{
    printer->print(this);
}

// CharLiteralExpression
void weasel::CharLiteralExpression::print(Printer *printer)
{
    fmt::print("Unimplemented char printer");
}

// ArrayLiteralExpression
void weasel::ArrayLiteralExpression::print(Printer *printer)
{
    printer->print(this);
}

// StringLiteralExpression
void weasel::StringLiteralExpression::print(Printer *printer)
{
    printer->print(this);
}

// NilLiteralExpression
void weasel::NilLiteralExpression::print(Printer *printer)
{
    fmt::print("Unimplemented nil literal printer");
}

// Statement
void weasel::DeclarationStatement::print(Printer *printer)
{
    printer->print(this);
}

// Expression
void weasel::VariableExpression::print(Printer *printer)
{
    printer->print(this);
}

// FieldExpression
void weasel::FieldExpression::print(Printer *printer)
{
    printer->print(this);
}

// ArrayExpression
void weasel::ArrayExpression::print(Printer *printer)
{
    printer->print(this);
}

// LogicalExpression
void weasel::LogicalExpression::print(Printer *printer)
{
    fmt::print("Unimplemented logical printer");
}

// ArithmeticExpression
void weasel::ArithmeticExpression::print(Printer *printer)
{
    printer->print(this);
}

// AssignmentExpression
void weasel::AssignmentExpression::print(Printer *printer)
{
    printer->print(this);
}

// ComparisonExpression
void weasel::ComparisonExpression::print(Printer *printer)
{
    printer->print(this);
}

// CallExpression
void weasel::CallExpression::print(Printer *printer)
{
    printer->print(this);
}

// ReturnExpression
void weasel::ReturnExpression::print(Printer *printer)
{
    printer->print(this);
}

// StructExpression
void weasel::StructExpression::print(Printer *printer)
{
    fmt::print("Unimplemented struct printer");
}

// BreakExpression
void weasel::BreakExpression::print(Printer *printer)
{
    fmt::print("Unimplemented break printer");
}

// ContinueExpression
void weasel::ContinueExpression::print(Printer *printer)
{
    fmt::print("Unimplemented continue printer");
}

// CompoundStatement
void weasel::CompoundStatement::print(Printer *printer)
{
    printer->print(this);
}

// ConditionStatement
void weasel::ConditionStatement::print(Printer *printer)
{
    printer->print(this);
}

// LoopingStatement
void weasel::LoopingStatement::print(Printer *printer)
{
    printer->print(this);
}

// Operator Expression
void weasel::UnaryExpression::print(Printer *printer)
{
    printer->print(this);
}

// TypeCastExpression
void weasel::TypeCastExpression::print(Printer *printer)
{
    fmt::print("Unimplemented type cast printer");
}

// Funtion
void weasel::Function::print(Printer *printer)
{
    printer->print(this);
}

// Literal
void weasel::NumberLiteralExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// DoubleLiteralExpression
void weasel::DoubleLiteralExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// FloatLiteralExpression
void weasel::FloatLiteralExpression::printAsOperand(Printer *printer)
{
    fmt::print("Unimplemented float literal printer");
}

// BoolLiteralExpression
void weasel::BoolLiteralExpression::printAsOperand(Printer *printer)
{
    printer->print(this);
}

// CharLiteralExpression
void weasel::CharLiteralExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// ArrayLiteralExpression
void weasel::ArrayLiteralExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// StringLiteralExpression
void weasel::StringLiteralExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// NilLiteralExpression
void weasel::NilLiteralExpression::printAsOperand(Printer *printer)
{
    fmt::print("Unimplemented nil literal printer");
}

// Statement
void weasel::DeclarationStatement::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// Expression
void weasel::VariableExpression::printAsOperand(Printer *printer)
{
    printer->print(this);
}

// FieldExpression
void weasel::FieldExpression::printAsOperand(Printer *printer)
{
    printer->print(this);
}

// ArrayExpression
void weasel::ArrayExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// LogicalExpression
void weasel::LogicalExpression::printAsOperand(Printer *printer)
{
    fmt::print("Unimplemented logical printer");
}

// ArithmeticExpression
void weasel::ArithmeticExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// AssignmentExpression
void weasel::AssignmentExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// ComparisonExpression
void weasel::ComparisonExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// CallExpression
void weasel::CallExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// ReturnExpression
void weasel::ReturnExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// BreakExpression
void weasel::BreakExpression::printAsOperand(Printer *printer)
{
    fmt::print("Unimplemented break printer");
}

// ContinueExpression
void weasel::ContinueExpression::printAsOperand(Printer *printer)
{
    fmt::print("Unimplemented continue printer");
}

// CompoundStatement
void weasel::CompoundStatement::printAsOperand(Printer *printer)
{
    fmt::print("Unimplemented compound printer");
}

// ConditionStatement
void weasel::ConditionStatement::printAsOperand(Printer *printer)
{
    fmt::print("Unimplemented condition printer");
}

// LoopingStatement
void weasel::LoopingStatement::printAsOperand(Printer *printer)
{
    fmt::print("Unimplemented looping printer");
}

// Operator Expression
void weasel::UnaryExpression::printAsOperand(Printer *printer)
{
    printer->printAsOperand(this);
}

// TypeCastExpression
void weasel::TypeCastExpression::printAsOperand(Printer *printer)
{
    fmt::print("Unimplemented type cast printer");
}
