#include <fmt/core.h>

#include <weasel/Printer/Printer.h>
#include <weasel/AST/AST.h>
#include <weasel/IR/Codegen.h>

// Literal
void weasel::NumberLiteralExpression::print(Printer *printer)
{
    printer->print(this);
}

void weasel::DoubleLiteralExpression::print(Printer *printer)
{
    // this->printDebug("Double Literal", shift);
}

void weasel::FloatLiteralExpression::print(Printer *printer)
{
    // this->printDebug("Float Literal", shift);
}

void weasel::BoolLiteralExpression::print(Printer *printer)
{
    // this->printDebug("Bool Literal", shift);
}

void weasel::CharLiteralExpression::print(Printer *printer)
{
    // this->printDebug("Char Literal", shift);
}

void weasel::ArrayLiteralExpression::print(Printer *printer)
{
    // this->printDebug("Array Literal", shift);
}

void weasel::StringLiteralExpression::print(Printer *printer)
{
    printer->print(this);
}

// TODO: Not Implemented Yet
void weasel::NilLiteralExpression::print(Printer *printer)
{
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

void weasel::FieldExpression::print(Printer *printer)
{
    // auto val = this->getParentField()->getToken().getValue() + "." + this->getField();
    // this->printDebug(val, shift);
}

void weasel::ArrayExpression::print(Printer *printer)
{
    // auto val = "Array Variable of " + this->getIdentifier();
    // this->printDebug(val, shift);
}

void weasel::LogicalExpression::print(Printer *printer)
{
    // auto op = this->getOperator().getValue();
    // this->printDebug("Logical Operation : " + op, shift);
    // this->getLHS()->debug(shift + DEFAULT_SHIFT);
    // this->getRHS()->debug(shift + DEFAULT_SHIFT);
}

void weasel::ArithmeticExpression::print(Printer *printer)
{
    // auto op = this->getOperator().getValue();
    // this->printDebug("Arithmetic Operation : " + op, shift);
    // this->getLHS()->debug(shift + DEFAULT_SHIFT);
    // this->getRHS()->debug(shift + DEFAULT_SHIFT);
}

void weasel::AssignmentExpression::print(Printer *printer)
{
    printer->print(this);
}

void weasel::ComparisonExpression::print(Printer *printer)
{
    printer->print(this);
    // this->printDebug("Comparison Operation : " + getToken().getValue(), shift);
    // this->getLHS()->debug(shift + DEFAULT_SHIFT);
    // this->getRHS()->debug(shift + DEFAULT_SHIFT);
}

void weasel::CallExpression::print(Printer *printer)
{
    printer->print(this);
}

void weasel::ReturnExpression::print(Printer *printer)
{
    printer->print(this);
    // auto val = fmt::format("return {}", this->getValue()->getType()->getTypeName());
    // this->printDebug(val, shift);
}

void weasel::StructExpression::print(Printer *printer)
{
    // auto type = dynamic_cast<StructType *>(this->getType());
    // auto val = "Struct " + type->getIdentifier();

    // val += "{";
    // for (auto &item : this->getFields())
    // {
    //     val += item->getIdentifier() + ",";
    // }
    // val += "}";

    // this->printDebug(val, shift);
}

void weasel::BreakExpression::print(Printer *printer)
{
    // if (getValue() == nullptr)
    // {
    //     return this->printDebug("Break Empty", shift);
    // }

    // this->printDebug("Break with Condition", shift);
    // getValue()->debug(shift + DEFAULT_SHIFT);
}

void weasel::ContinueExpression::print(Printer *printer)
{
    // if (getValue() == nullptr)
    // {
    //     return this->printDebug("Continue Empty", shift);
    // }

    // this->printDebug("Continue with Condition", shift);
    // getValue()->debug(shift + DEFAULT_SHIFT);
}

// Statement
void weasel::CompoundStatement::print(Printer *printer)
{
    printer->print(this);
}

void weasel::ConditionStatement::print(Printer *printer)
{
    printer->print(this);
}

void weasel::LoopingStatement::print(Printer *printer)
{
    // this->printDebug("Looping Statement", shift);
    // if (_conditions.empty())
    // {
    //     this->printDebug("Infinite Condition", shift);
    // }
    // else
    // {
    //     this->printDebug("Start Condition", shift);
    //     for (auto &item : _conditions)
    //     {
    //         item->debug(shift + DEFAULT_SHIFT);
    //     }

    //     this->printDebug("End Condition", shift);
    // }

    // _body->debug(shift + DEFAULT_SHIFT);
}

// Operator Expression
void weasel::UnaryExpression::print(Printer *printer)
{
    // auto op = this->getToken().getValue();

    // this->printDebug("Unary Expression (" + op + ")", shift);
    // this->getExpression()->debug(shift + DEFAULT_SHIFT);
}

void weasel::TypeCastExpression::print(Printer *printer)
{
    // auto op = this->getToken().getValue();

    // this->printDebug("Type Cast Expression (" + op + ")", shift);
    // this->getExpression()->debug(shift + DEFAULT_SHIFT);
}

// Funtion Debug
void weasel::Function::print(Printer *printer)
{
    printer->print(this);
}
