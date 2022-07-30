#include <iostream>

#include "weasel/Basic/Number.h"
#include "weasel/Parser/Parser.h"
#include "weasel/Symbol/Symbol.h"

void weasel::Parser::ignoreNewline()
{
    if (getCurrentToken().isNewline())
    {
        getNextToken(true);
    }
}

weasel::Type *weasel::Parser::parseDataType()
{
    // Pointer
    if (getCurrentToken().isKind(TokenKind::TokenOperatorStar))
    {
        getNextToken(); // eat '*'

        auto containedType = parseDataType();
        if (containedType == nullptr)
        {
            return ErrorTable::addError(getCurrentToken(), "Expected data type after pointer type");
        }

        return Type::getPointerType(containedType);
    }

    // Array
    if (getCurrentToken().isKind(TokenKind::TokenDelimOpenSquareBracket))
    {
        if (!getNextToken().isKind(TokenKind::TokenLitInteger))
        {
            return ErrorTable::addError(getCurrentToken(), "Expected size of array");
        }

        auto numStr = getCurrentToken().getValue();
        if (!weasel::Number::isInteger(numStr))
        {
            return ErrorTable::addError(getCurrentToken(), "Number is not a valid integer");
        }

        if (!getNextToken().isKind(TokenKind::TokenDelimCloseSquareBracket))
        {
            return ErrorTable::addError(getCurrentToken(), "Expected ] for array type");
        }

        if (!getNextToken().isDataType())
        {
            return ErrorTable::addError(getCurrentToken(), "Expected data type after [...]");
        }

        // Eat Current Token
        getNextToken();

        auto num = Number::toInteger(numStr);
        auto containedType = Type::create(getCurrentToken());

        return Type::getArrayType(containedType, num);
    }

    // Check User Type
    auto userType = findUserType(getCurrentToken().getValue());
    if (userType != nullptr)
    {
        getNextToken(); // remove current User Type
        return userType;
    }

    // Normal Data Type or no datatype
    auto type = Type::create(getCurrentToken());
    if (type != nullptr)
    {
        // Remove Current Token
        getNextToken();
    }

    return type;
}
