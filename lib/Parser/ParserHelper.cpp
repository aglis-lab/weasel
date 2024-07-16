#include "weasel/Basic/Number.h"
#include "weasel/Parser/Parser.h"

#include <cassert>

void weasel::Parser::ignoreNewline()
{
    if (getCurrentToken().isNewline())
    {
        getNextToken(true);
    }
}

TypeHandle Parser::parseDataType()
{
    // Pointer
    if (getCurrentToken().isKind(TokenKind::TokenOperatorStar))
    {
        getNextToken(); // eat '*'

        auto containedType = parseDataType();

        return Type::getPointerType(move(containedType));
    }

    // Address of type
    if (getCurrentToken().isKind(TokenKind::TokenOperatorAnd))
    {
        getNextToken(); // eat '&'

        auto containedType = parseDataType();

        return Type::getReferenceType(move(containedType));
    }

    // Array
    if (getCurrentToken().isKind(TokenKind::TokenDelimOpenSquareBracket))
    {
        auto arraySize = -1;
        if (getNextToken().isKind(TokenKind::TokenLitInteger))
        {
            auto numStr = getCurrentToken().getValue();

            assert(weasel::Number::isInteger(numStr) && "Number is not a valid integer");

            arraySize = (int)Number::toInteger(numStr);

            getNextToken(); // eat 'integer'
        }

        assert(getCurrentToken().isKind(TokenKind::TokenDelimCloseSquareBracket) && "Expected ] for array type");

        getNextToken(); // eat ']'
        auto containedType = parseDataType();

        return Type::getArrayType(move(containedType), arraySize);
    }

    // Normal Data Type or no datatype
    auto type = Type::create(getCurrentToken());

    // Remove Current Token
    getNextToken();

    return type;
}

weasel::Expression *weasel::Parser::createOperatorExpression(Token op, Expression *lhs, Expression *rhs)
{
    LOG(INFO) << "Parse Create Operator Expression...";

    return nullptr;
    // if (op.isOperatorAssignment())
    // {
    //     if (!op.isOperatorEqual())
    //     {
    //         auto startBuffer = op.getStartBuffer();
    //         auto endBuffer = op.getEndBuffer() - 1;
    //         auto tokenKind = TokenKind::TokenUnknown;

    //         if (auto tokenVal = std::string(startBuffer, endBuffer); tokenVal == "-")
    //             tokenKind = TokenKind::TokenOperatorPlus;
    //         else if (tokenVal == "*")
    //             tokenKind = TokenKind::TokenOperatorStar;
    //         else if (tokenVal == "/")
    //             tokenKind = TokenKind::TokenOperatorSlash;
    //         else if (tokenVal == "%")
    //             tokenKind = TokenKind::TokenOperatorPercent;
    //         else if (tokenVal == "^")
    //             tokenKind = TokenKind::TokenOperatorCaret;
    //         else if (tokenVal == "|")
    //             tokenKind = TokenKind::TokenOperatorOr;
    //         else if (tokenVal == "&")
    //             tokenKind = TokenKind::TokenOperatorAnd;
    //         else if (tokenVal == ">>")
    //             tokenKind = TokenKind::TokenOperatorShiftRight;
    //         else if (tokenVal == "<<")
    //             tokenKind = TokenKind::TokenOperatorShiftLeft;

    //         assert(tokenKind != TokenKind::TokenUnknown);

    //         auto token = Token::create(tokenKind, op.getLocation(), startBuffer, endBuffer);

    //         rhs = new ArithmeticExpression(token, lhs, rhs);
    //     }

    //     return new AssignmentExpression(op, lhs, rhs);
    // }

    // if (op.isComparison())
    // {
    //     return new ComparisonExpression(op, lhs, rhs);
    // }

    // if (op.isOperatorLogical())
    // {
    //     return new LogicalExpression(op, lhs, rhs);
    // }

    // return new ArithmeticExpression(op, lhs, rhs);
}
