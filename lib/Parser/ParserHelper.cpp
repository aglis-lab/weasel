#include "weasel/Basic/Number.h"
#include "weasel/Parser/Parser.h"

#include <cassert>

void Parser::ignoreNewline()
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

            assert(Number::isInteger(numStr) && "Number is not a valid integer");

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

ExpressionHandle Parser::createOperatorExpression(Token op, ExpressionHandle lhs, ExpressionHandle rhs)
{
    LOG(INFO) << "Parse Create Operator Expression...";

    if (op.isOperatorAssignment())
    {
        if (!op.isOperatorEqual())
        {
            auto startBuffer = op.getStartBuffer();
            auto endBuffer = op.getEndBuffer() - 1;
            auto tokenKind = TokenKind::TokenUnknown;

            if (auto tokenVal = string_view(startBuffer, endBuffer); tokenVal == "-")
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

            auto token = Token::create(tokenKind, op.getLocation(), startBuffer, endBuffer);

            rhs = make_shared<ArithmeticExpression>(token, lhs, rhs);
        }

        return make_shared<AssignmentExpression>(op, lhs, rhs);
    }

    if (op.isComparison())
    {
        return make_shared<ComparisonExpression>(op, lhs, rhs);
    }

    if (op.isOperatorLogical())
    {
        return make_shared<LogicalExpression>(op, lhs, rhs);
    }

    return make_shared<ArithmeticExpression>(op, lhs, rhs);
}
