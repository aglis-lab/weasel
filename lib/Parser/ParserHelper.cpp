#include "weasel/Basic/Number.h"
#include "weasel/Parser/Parser.h"
#include "weasel/Symbol/Symbol.h"
#include <iostream>

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
        if (!getNextToken().isDataType())
        {
            return ErrorTable::addError(getCurrentToken(), "Expected data type after pointer type");
        }

        auto containedType = Type::create(getCurrentToken());
        auto type = Type::getPointerType(containedType);

        // Remove current token data type
        getNextToken();

        return type;
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

    // Normal Data Type or no datatype
    // auto type = getCurrentToken().toType();
    auto type = Type::create(getCurrentToken());
    if (type != nullptr)
    {
        // Remove Current Token
        getNextToken();
    }

    return type;
}
