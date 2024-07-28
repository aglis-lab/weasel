#include <weasel/AST/AST.h>
#include <weasel/IR/Codegen.h>

bool weasel::Expression::isCompoundExpression() const
{
    return typeid(CompoundStatement) == typeid(*this);
}

bool weasel::Expression::isStructExpression() const
{
    return typeid(StructExpression) == typeid(*this);
}

// Function
std::string weasel::Function::getManglingName()
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
