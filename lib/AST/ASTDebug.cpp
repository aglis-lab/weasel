#include <iostream>
#include <iomanip>
#include "weasel/AST/AST.h"
#include "weasel/IR/Context.h"

// Internal Printing
void weasel::ASTDebug::printDebug(const std::string &val, int shift)
{
    std::cout << std::setfill(' ') << std::setw(shift);
    std::cout << "> " << val << std::endl;
}

// Literal
void weasel::NumberLiteralExpression::debug(int shift)
{
    this->printDebug("Number Literal", shift);
}

void weasel::DoubleLiteralExpression::debug(int shift)
{
    this->printDebug("Double Literal", shift);
}

void weasel::FloatLiteralExpression::debug(int shift)
{
    this->printDebug("Float Literal", shift);
}

void weasel::BoolLiteralExpression::debug(int shift)
{
    this->printDebug("Bool Literal", shift);
}

void weasel::CharLiteralExpression::debug(int shift)
{
    this->printDebug("Char Literal", shift);
}

void weasel::ArrayLiteralExpression::debug(int shift)
{
    this->printDebug("Array Literal", shift);
}

void weasel::StringLiteralExpression::debug(int shift)
{
    this->printDebug("String Literal", shift);
}

// Statement
void weasel::DeclarationExpression::debug(int shift)
{
    auto val = "Declaration " + this->getIdentifier();
    this->printDebug(val, shift);
}

void weasel::StatementExpression::debug(int shift)
{
    for (auto &item : this->getBody())
    {
        item->debug(shift + this->defaultShift);
    }
}

void weasel::ConditionStatementExpression::debug(int shift)
{
    this->printDebug("If Statement", shift);
    _condition->debug(shift);

    _statement->debug(shift + this->defaultShift);
}

void weasel::VariableExpression::debug(int shift)
{
    auto val = "Variable " + this->getIdentifier();
    this->printDebug(val, shift);
}

void weasel::ArrayExpression::debug(int shift)
{
    auto val = "Array " + this->getIdentifier();
    this->printDebug(val, shift);
}

void weasel::BinaryOperatorExpression::debug(int shift)
{
    auto op = this->getOperator().getValue();
    this->printDebug("Binary Operation : " + op, shift);
    this->getLHS()->debug(shift + this->defaultShift);
    this->getRHS()->debug(shift + this->defaultShift);
}

void weasel::CallExpression::debug(int shift)
{
    auto val = this->getIdentifier();
    this->printDebug("Call : " + val, shift);
}

void weasel::ReturnExpression::debug(int shift)
{
    this->printDebug("Return", shift);
}

// TODO: Not Implemented Yet
void weasel::NilLiteralExpression::debug(int shift)
{
}

// Funtion Debug
void weasel::Function::debug(int shift)
{
    this->printDebug("Function : " + this->getIdentifier(), shift);
    this->getBody()->debug(shift + this->defaultShift);
}
