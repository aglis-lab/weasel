#include <weasel/AST/AST.h>

using namespace weasel;

#define UNIMPLEMENTED LOG(INFO) << "Unimplemented Error Checking"

bool ArgumentExpression::isError()
{
    UNIMPLEMENTED;
    return false;
}

bool GlobalVariable::isError()
{
    return _error.has_value() || getValue()->isError();
}

bool MethodCallExpression::isError()
{
    for (auto &item : getArguments())
    {
        if (item->isError())
        {
            return true;
        }
    }

    return _error.has_value();
}

bool NumberLiteralExpression::isError()
{
    return _error.has_value();
}

bool DoubleLiteralExpression::isError()
{
    return _error.has_value();
}

bool FloatLiteralExpression::isError()
{
    return _error.has_value();
}

bool BoolLiteralExpression::isError()
{
    return _error.has_value();
}

bool CharLiteralExpression::isError()
{
    return _error.has_value();
}

bool ArrayExpression::isError()
{
    return _error.has_value();
}

bool StringLiteralExpression::isError()
{
    return _error.has_value();
}

bool NilLiteralExpression::isError()
{
    return _error.has_value();
}

bool DeclarationStatement::isError()
{
    return _error.has_value() || getValue()->isError();
}

bool VariableExpression::isError()
{
    return _error.has_value();
}

bool FieldExpression::isError()
{
    return _error.has_value() || getParentField()->isError();
}

bool IndexExpression::isError()
{
    return _error.has_value();
}

bool ArithmeticExpression::isError()
{
    return _error.has_value() || getLHS()->isError() || getRHS()->isError();
}

bool LogicalExpression::isError()
{
    return _error.has_value() || getLHS()->isError() || getRHS()->isError();
}

bool AssignmentExpression::isError()
{
    return _error.has_value() || getLHS()->isError() || getRHS()->isError();
}

bool ComparisonExpression::isError()
{
    return _error.has_value() || getLHS()->isError() || getRHS()->isError();
}

bool CallExpression::isError()
{
    for (auto &item : getArguments())
    {
        if (item->isError())
        {
            return true;
        }
    }

    return _error.has_value() || getFunction()->isError();
}

bool ReturnExpression::isError()
{
    if (getValue() && getValue()->isError())
    {
        return true;
    }

    return _error.has_value();
}

bool BreakExpression::isError()
{
    if (getValue() && getValue()->isError())
    {
        return true;
    }

    return _error.has_value();
}

bool ContinueExpression::isError()
{
    if (getValue() && getValue()->isError())
    {
        return true;
    }

    return _error.has_value();
}

bool StructExpression::isError()
{
    for (auto &item : getFields())
    {
        if (item->getValue()->isError())
        {
            return true;
        }
    }

    return _error.has_value();
}

bool Function::isError()
{
    for (auto &item : getArguments())
    {
        if (item->isError())
        {
            return true;
        }
    }

    return _error.has_value();
}

bool UnaryExpression::isError()
{
    return _error.has_value() || getValue()->isError();
}

bool TypeCastExpression::isError()
{
    return _error.has_value() || getValue()->isError();
}

bool CompoundStatement::isError()
{
    return _error.has_value();
}

bool ConditionStatement::isError()
{
    return _error.has_value();
}

bool LoopingStatement::isError()
{
    return _error.has_value() || getBody()->isError();
}
