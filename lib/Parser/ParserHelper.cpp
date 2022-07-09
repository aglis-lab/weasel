#include <llvm/IR/DerivedTypes.h>
#include "weasel/Basic/Number.h"
#include "weasel/Parser/Parser.h"
#include "weasel/Symbol/Symbol.h"

void weasel::Parser::ignoreNewline()
{
    if (getCurrentToken()->isNewline())
    {
        getNextToken(true);
    }
}

llvm::Type *weasel::Parser::parseDataType()
{
    // Pointer
    if (getCurrentToken()->isKind(TokenKind::TokenOperatorStar))
    {
        if (!getNextToken()->isDataType())
        {
            return ErrorTable::addError(getCurrentToken(), "Expected data type after pointer type");
        }

        auto *type = getCurrentToken()->toType(*getContext(), true);

        getNextToken();
        return type;
    }

    // Array
    if (getCurrentToken()->isKind(TokenKind::TokenDelimOpenSquareBracket))
    {
        if (!getNextToken()->isKind(TokenKind::TokenLitNumber))
        {
            return ErrorTable::addError(getCurrentToken(), "Expected size of array");
        }

        auto numStr = getCurrentToken()->getValue();
        if (!weasel::Number::isInteger(numStr))
        {
            return ErrorTable::addError(getCurrentToken(), "Number is not a valid integer");
        }

        if (!getNextToken()->isKind(TokenKind::TokenDelimCloseSquareBracket))
        {
            return ErrorTable::addError(getCurrentToken(), "Expected ] for array type");
        }

        if (!getNextToken()->isDataType())
        {
            return ErrorTable::addError(getCurrentToken(), "Expected data type after [...]");
        }

        auto num = weasel::Number::toInteger(numStr);
        auto *type = getCurrentToken()->toType(*getContext());
        auto *arrTy = llvm::ArrayType::get(type, num);

        getNextToken();

        return arrTy;
    }

    // Normal Data Type or no datatype
    auto *type = getCurrentToken()->toType(*getContext());
    if (type == nullptr)
    {
        return nullptr;
    }
    getNextToken();

    return type;
}
