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

// TODO: Not Implemented Yet
void weasel::NilLiteralExpression::debug(int shift)
{
}

// Statement
void weasel::DeclarationStatement::debug(int shift)
{
    this->printDebug("Declaration " + this->getIdentifier(), shift);
    if (this->getValue() != nullptr)
    {
        this->getValue()->debug(shift + defaultShift);
    }
}

// Expression
void weasel::VariableExpression::debug(int shift)
{
    auto val = "Variable " + this->getIdentifier();
    this->printDebug(val, shift);
}

void weasel::FieldExpression::debug(int shift)
{
    auto val = this->getParent()->getToken().getValue() + "." + this->getField();
    this->printDebug(val, shift);
}

void weasel::ArrayExpression::debug(int shift)
{
    auto val = "Array " + this->getIdentifier();
    this->printDebug(val, shift);
}

void weasel::BinaryExpression::debug(int shift)
{

    auto op = this->getOperator().getValue();
    this->printDebug("Binary Operation : " + op, shift);
    this->getLHS()->debug(shift + this->defaultShift);
    this->getRHS()->debug(shift + this->defaultShift);
}

void weasel::CallExpression::debug(int shift)
{
    auto val = this->getIdentifier();
    this->printDebug("Call " + val, shift);
}

void weasel::ReturnExpression::debug(int shift)
{
    this->printDebug("Return", shift);
}

void weasel::StructExpression::debug(int shift)
{
    auto val = "Struct " + this->getType()->getIdentifier();

    val += "{";
    for (auto &item : this->getFields())
    {
        val += item->getIdentifier() + ",";
    }
    val += "}";

    this->printDebug(val, shift);
}

void weasel::BreakExpression::debug(int shift)
{
    if (getValue() == nullptr)
    {
        return this->printDebug("Break Empty", shift);
    }

    this->printDebug("Break with Condition", shift);
    getValue()->debug(shift + defaultShift);
}

void weasel::ContinueExpression::debug(int shift)
{
    if (getValue() == nullptr)
    {
        return this->printDebug("Continue Empty", shift);
    }

    this->printDebug("Continue with Condition", shift);
    getValue()->debug(shift + defaultShift);
}

// Statement
void weasel::CompoundStatement::debug(int shift)
{
    for (auto &item : this->getBody())
    {
        item->debug(shift + this->defaultShift);
    }
}

void weasel::ConditionStatement::debug(int shift)
{
    this->printDebug("If Statement", shift);
    this->printDebug("If Conditions", shift);
    for (auto &item : getConditions())
    {
        item->debug(shift + defaultShift);
    }

    this->printDebug("If Body", shift);
    for (auto &item : getStatements())
    {
        this->printDebug("Body", shift);
        item->debug(shift + defaultShift);
    }
}

void weasel::LoopingStatement::debug(int shift)
{
    this->printDebug("Looping Statement", shift);
    if (_conditions.empty())
    {
        this->printDebug("Infinite Condition", shift);
    }
    else
    {
        this->printDebug("Start Condition", shift);
        for (auto &item : _conditions)
        {
            item->debug(shift + this->defaultShift);
        }

        this->printDebug("End Condition", shift);
    }

    _body->debug(shift + this->defaultShift);
}

// Operator Expression
void weasel::UnaryExpression::debug(int shift)
{
    auto op = this->getToken().getValue();

    this->printDebug("Unary Expression (" + op + ")", shift);
    this->getExpression()->debug(shift + defaultShift);
}

// Funtion Debug
void weasel::Function::debug(int shift)
{
    this->printDebug("Function : " + this->getIdentifier(), shift);
    this->getBody()->debug(shift + this->defaultShift);
}
