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

    // Reference of type
    if (getCurrentToken().isKind(TokenKind::TokenOperatorAnd))
    {
        getNextToken(); // eat '&'

        auto containedType = parseDataType();

        return Type::getReferenceType(move(containedType));
    }

    // Array
    if (getCurrentToken().isKind(TokenKind::TokenDelimOpenSquareBracket))
    {
        auto array = make_shared<ArrayType>();
        getNextToken(); // eat '['
        if (!getCurrentToken().isCloseSquare())
        {
            array->setSize(parseExpressionWithoutBlock());
        }
        else
        {
            array->setWidth(-1);
        }

        assert(getCurrentToken().isKind(TokenKind::TokenDelimCloseSquareBracket) && "Expected ] for array type");
        getNextToken(); // eat ']'

        auto containedType = parseDataType();
        assert(containedType);

        array->setContainedType(containedType);
        return array;
    }

    // Lambda Type
    if (getCurrentToken().isKeyFunction())
    {
        auto funToken = getCurrentToken();
        getNextToken(); // eat 'fun'

        // TODO: Create Better Error for Token
        if (!getCurrentToken().isOpenParen())
        {
            assert(false && "Lambda should be followed with open paren");
        }

        auto type = make_shared<FunctionType>();
        type->setToken(funToken);
        do
        {
            // eat ',' or '('
            if (!getNextToken().isDataType())
            {
                // TODO: Create Better Error for Token
                assert(false && "Lambda should be followed with open paren");
            }

            if (getCurrentToken().isCloseParen())
            {
                break;
            }

            auto argType = parseDataType();

            type->getArguments().push_back(argType);
            if (argType->isError())
            {
                type->setError(argType->getError().value());
                return type;
            }
        } while (getCurrentToken().isComma());

        // eat ')'
        if (getNextToken().isDataType())
        {
            type->setReturnType(parseDataType());
        }
        else
        {
            type->setReturnType(Type::getVoidType());
        }

        return type;
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
