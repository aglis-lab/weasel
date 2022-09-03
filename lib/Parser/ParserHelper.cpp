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
        auto arraySize = -1;
        if (getNextToken().isKind(TokenKind::TokenLitInteger))
        {
            auto numStr = getCurrentToken().getValue();
            if (!weasel::Number::isInteger(numStr))
            {
                return ErrorTable::addError(getCurrentToken(), "Number is not a valid integer");
            }

            arraySize = Number::toInteger(numStr);

            getNextToken(); // eat 'integer'
        }

        if (!getCurrentToken().isKind(TokenKind::TokenDelimCloseSquareBracket))
        {
            return ErrorTable::addError(getCurrentToken(), "Expected ] for array type");
        }

        getNextToken(); // eat ']'

        auto containedType = parseDataType();
        if (containedType == nullptr)
        {
            return nullptr;
        }

        return Type::getArrayType(containedType, arraySize);
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

weasel::Expression *weasel::Parser::createOperatorExpression(Token op, Expression *lhs, Expression *rhs)
{
    if (op.isOperatorAssignment())
    {
        if (!op.isOperatorEqual())
        {
            auto startBuffer = op.getStartBuffer();
            auto endBuffer = op.getEndBuffer() - 1;
            auto tokenKind = TokenKind::TokenUnknown;
            auto tokenVal = std::string(startBuffer, endBuffer);

            if (tokenVal == "-")
                tokenKind = TokenKind::TokenOperatorPlus;
            else if (tokenVal == "*")
                tokenKind = TokenKind::TokenOperatorStar;
            else if (tokenVal == "/")
                tokenKind = TokenKind::TokenOperatorSlash;
            else if (tokenVal == "%")
                tokenKind = TokenKind::TokenOperatorPercent;
            else if (tokenVal == "^")
                tokenKind = TokenKind::TokenOperatorCaret;
            else if (tokenVal == "|")
                tokenKind = TokenKind::TokenOperatorOr;
            else if (tokenVal == "&")
                tokenKind = TokenKind::TokenOperatorAnd;
            else if (tokenVal == ">>")
                tokenKind = TokenKind::TokenOperatorShiftRight;
            else if (tokenVal == "<<")
                tokenKind = TokenKind::TokenOperatorShiftLeft;

            assert(tokenKind != TokenKind::TokenUnknown);

            auto token = Token(tokenKind, op.getLocation(), startBuffer, endBuffer);

            rhs = new ArithmeticExpression(token, lhs, rhs);
        }

        return new AssignmentExpression(op, lhs, rhs);
    }

    if (op.isComparison())
    {
        return new ComparisonExpression(op, lhs, rhs);
    }

    if (op.isOperatorLogical())
    {
        return new LogicalExpression(op, lhs, rhs);
    }

    return new ArithmeticExpression(op, lhs, rhs);
}
