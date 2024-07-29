#include <weasel/AST/AST.h>
#include <weasel/IR/Codegen.h>

bool Expression::isCompoundExpression() const
{
    return typeid(CompoundStatement) == typeid(*this);
}

bool Expression::isStructExpression() const
{
    return typeid(StructExpression) == typeid(*this);
}

bool Expression::isFieldExpression() const
{
    return typeid(FieldExpression) == typeid(*this);
}

bool Expression::isNilExpression() const
{
    return typeid(NilLiteralExpression) == typeid(*this);
}

// Function
std::string Function::getManglingName()
{
    if (this->isExtern() || this->isMain())
    {
        return this->getIdentifier();
    }

    std::string mangleName;

    // Reserve 50 character
    mangleName.reserve(50);

    // First prefix
    mangleName += "_W";

    // Impl Struct
    if (this->isImplStructExist())
    {
        mangleName += "I" + this->getImplStruct()->getManglingName();
    }

    // Return Type
    mangleName += "R" + this->getType()->getManglingName();

    // Function Name with prefix
    mangleName += "7" + this->getIdentifier();

    // Arguments with prefix
    mangleName += "3";
    for (auto item : this->getArguments())
    {
        mangleName += item->getType()->getManglingName();
    }

    // End of argument or function
    mangleName += "_";

    // Return those manglename
    return mangleName;
}
