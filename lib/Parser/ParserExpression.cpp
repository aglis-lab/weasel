#include <iostream>

#include "weasel/Parser/Parser.h"
#include <weasel/Basic/Error.h>

weasel::GlobalVariable *weasel::Parser::parseGlobalVariable()
{
    auto declToken = getCurrentToken();
    auto idenToken = getNextToken();

    getNextToken(); // eat '=' sign

    auto valueExpr = parseLiteralExpression();

    return new GlobalVariable(declToken, idenToken.getValue(), valueExpr);
}

weasel::Expression *weasel::Parser::parseMethodCallExpression(Expression *implExpression)
{
    auto callToken = getCurrentToken();
    auto expr = new MethodCallExpression(callToken);

    if (!getNextToken().isOpenParen())
    {
        expr->setError(Errors::getInstance().expectedOpenParen);

        return expr;
    }

    if (!getNextToken().isCloseParen())
    {
        while (true)
        {
            auto arg = parseExpression();
            expr->getArguments().push_back(arg);

            if (getCurrentToken().isCloseParen())
            {
                break;
            }

            if (!getCurrentToken().isComma())
            {
                expr->setError(Errors::getInstance().expectedCloseParen);
                return expr;
            }

            getNextToken();
        }
    }

    getNextToken(); // eat ')'

    StructType *structType;
    if (implExpression->getType()->isStructType())
    {
        structType = dynamic_cast<StructType *>(implExpression->getType());
    }
    else
    {
        structType = dynamic_cast<StructType *>(implExpression->getType()->getContainedType());
    }
    auto fun = findFunction(callToken.getValue(), structType);

    expr->setType(fun->getType());
    expr->setImplExpression(implExpression);
    expr->setFunction(fun);

    return expr;
}

weasel::Expression *weasel::Parser::parseStaticMethodCallExpression()
{
    getNextToken(); // eat last Struct or identifier

    // eat '.' and check next token
    if (!getNextToken().isDot())
    {
        return new ErrorExpression(getCurrentToken(), Errors::getInstance().expectedDot);
    }

    // Call Expression
    return parseCallExpression();
}

weasel::Expression *weasel::Parser::parseLiteralExpression()
{
    auto token = getCurrentToken();

    getNextToken(); // eat 'literal'

    if (token.isKind(TokenKind::TokenLitBool))
    {
        return new BoolLiteralExpression(token, token.getValue() == "true");
    }

    if (token.isKind(TokenKind::TokenLitChar))
    {
        auto val = token.getValue()[0];
        return new CharLiteralExpression(token, val);
    }

    if (token.isKind(TokenKind::TokenLitInteger))
    {
        auto value = strtoll(token.getValue().c_str(), nullptr, 10);
        return new NumberLiteralExpression(token, value);
    }

    if (token.isKind(TokenKind::TokenLitFloat))
    {
        auto value = std::stof(token.getValue());
        return new FloatLiteralExpression(token, value);
    }

    if (token.isKind(TokenKind::TokenLitDouble))
    {
        auto value = std::stod(token.getValue());
        return new DoubleLiteralExpression(token, value);
    }

    if (token.isKind(TokenKind::TokenLitString))
    {
        auto *currentBuffer = token.getStartBuffer();
        auto *endBuffer = token.getEndBuffer();
        std::string value = "";

        while (endBuffer - currentBuffer > 0)
        {
            auto currentChar = *currentBuffer;
            auto nextChar = *(currentBuffer + 1);

            if (currentChar == '\\' && nextChar == 'n')
            {
                value += '\n';
                currentBuffer += 2;
            }
            else
            {
                value += currentChar;
                currentBuffer += 1;
            }
        }

        return new StringLiteralExpression(token, value);
    }

    return new NilLiteralExpression(getCurrentToken());
}

weasel::Expression *weasel::Parser::parseCallExpression()
{
    auto callToken = getCurrentToken();
    auto expr = new CallExpression(callToken);
    if (!getNextToken().isOpenParen())
    {
        expr->setError(Errors::getInstance().funCallExpectedOpenParen);

        return expr;
    }

    if (!getNextToken().isCloseParen())
    {
        while (true)
        {
            auto arg = parseExpression();
            expr->getArguments().push_back(arg);

            if (getCurrentToken().isCloseParen())
            {
                break;
            }

            if (!getCurrentToken().isComma())
            {
                expr->setError(Errors::getInstance().expectedCloseParen);
                return expr;
            }

            getNextToken();
        }
    }

    getNextToken(); // eat ')'

    return expr;
}

weasel::Expression *weasel::Parser::parseIdentifierExpression()
{
    // Check Available Function
    auto identToken = getCurrentToken();
    auto identifier = identToken.getValue();
    if (getNextToken().isOpenParen())
    {
        return parseCallExpression();
    }

    if (getCurrentToken().isOpenSquare())
    {
        getNextToken(); // eat [
        auto indexExpr = parseExpression();
        if (!getCurrentToken().isCloseSquare())
        {
            return new ErrorExpression(indexExpr->getToken(), Errors::getInstance().expectedCloseSquare);
        }

        getNextToken(); // eat ]
        return new ArrayExpression(indexExpr->getToken(), identifier, indexExpr);
    }

    return new VariableExpression(identToken, identifier);
}

weasel::Expression *weasel::Parser::parseParenExpression()
{
    getNextToken(); // eat (
    auto expr = parseExpression();

    if (!getCurrentToken().isCloseParen())
    {
        expr->setError(Errors::getInstance().funCallExpectedOpenParen);

        return expr;
    }

    getNextToken(); // eat ')'

    return expr;
}

// Default Type
weasel::Expression *weasel::Parser::parseArrayExpression()
{
    LOG(INFO) << "Parsing Array\n";

    std::vector<weasel::Expression *> items;

    getNextToken(); // eat [
    while (!getCurrentToken().isKind(TokenKind::TokenDelimCloseSquareBracket))
    {
        items.push_back(parseLiteralExpression());
        if (getCurrentToken().isKind(TokenKind::TokenPuncComma))
        {
            getNextToken(); // eat ,
        }
    }

    getNextToken(); // eat ]
    return new ArrayLiteralExpression(items);
}

weasel::Expression *weasel::Parser::parseStructExpression()
{
    auto token = getCurrentToken(); // Identifier
    getNextToken();                 // Eat Identifier and next to '{'
    getNextToken(true);             // eat '{'

    auto expr = new StructExpression(token);
    while (!getCurrentToken().isCloseCurly())
    {
        auto idenToken = getCurrentToken();
        if (!idenToken.isIdentifier())
        {
            expr->setError(Errors::getInstance().expectedIdentifier.withToken(idenToken));
            return expr;
        }

        auto colonToken = getNextToken();
        if (!colonToken.isColon())
        {
            expr->setError(Errors::getInstance().expectedColon.withToken(colonToken));
            return expr;
        }

        auto exprToken = getNextToken(); // eat ':'
        auto valueExpr = parseExpression();
        auto field = new StructExpression::StructField(idenToken.getValue(), valueExpr);
        expr->getFields().push_back(field);

        ignoreNewline();
        if (getCurrentToken().isCloseCurly())
        {
            break;
        }

        if (!getCurrentToken().isComma())
        {
            expr->setError(Errors::getInstance().expectedComma.withToken(getCurrentToken()));
            return expr;
        }

        getNextToken(true); // eat ','
    }

    getNextToken(); // eat '}'

    return expr;
}

weasel::Expression *weasel::Parser::parsePrimaryExpression()
{
    auto possibleHaveField = getCurrentToken().isKeyThis() ||
                             getCurrentToken().isIdentifier() ||
                             getCurrentToken().isOpenParen() ||
                             getCurrentToken().isOpenSquare();

    if (possibleHaveField)
    {
        weasel::Expression *expr = nullptr;

        // Identifier
        // Call or Variable Expression or Struct Expression
        if (getCurrentToken().isIdentifier() || getCurrentToken().isKeyThis())
        {
            if (expectToken(TokenKind::TokenPuncDot))
            {
                expr = parseStaticMethodCallExpression();
            }
            else if (expectToken(TokenKind::TokenDelimOpenCurlyBracket))
            {
                expr = parseStructExpression();
            }
            else
            {
                expr = parseIdentifierExpression();
            }
        }

        // Parentise Expression
        else if (getCurrentToken().isOpenParen())
        {
            expr = parseParenExpression();
        }

        // Array Expression
        else if (getCurrentToken().isOpenSquare())
        {
            expr = parseArrayExpression();
        }

        // Check for possible Field Expression
        if (expr != nullptr && getCurrentToken().isDot())
        {
            expr = parseFieldExpression(expr);
        }

        return expr;
    }

    // Literal Expression
    if (getCurrentToken().isLiteral())
    {
        return parseLiteralExpression();
    }

    // Unary Expression
    if (getCurrentToken().isOperatorUnary())
    {
        auto token = getCurrentToken();

        getNextToken(); // eat ' & | * | - | ! | ~ '

        auto expr = parsePrimaryExpression();
        if (expr->isError())
        {
            skipUntilNewLine();
            return expr;
        }

        UnaryExpression::Operator op;
        switch (token.getTokenKind())
        {
        case TokenKind::TokenOperatorStar:
            op = UnaryExpression::Dereference;
            break;
        case TokenKind::TokenOperatorNegative:
            op = UnaryExpression::Negative;
            break;
        case TokenKind::TokenOperatorPlus:
            op = UnaryExpression::Positive;
            break;
        case TokenKind::TokenOperatorNot:
            op = UnaryExpression::Not;
            break;
        case TokenKind::TokenOperatorNegation:
            op = UnaryExpression::Negation;
            break;
        case TokenKind::TokenOperatorAnd:
            op = UnaryExpression::Borrow;
            break;
        default:
            op = UnaryExpression::Positive;
        }

        return new UnaryExpression(token, op, expr);
    }

    return new ErrorExpression(getCurrentToken(), Errors::getInstance().expectedExpression.withToken(getCurrentToken()));
}

weasel::Expression *weasel::Parser::parseExpression()
{
    auto lhs = parsePrimaryExpression();
    if (lhs->isError())
    {
        skipUntilNewLine();
        return lhs;
    }

    return parseExpressionOperator(__defaultPrecOrder, lhs);
}

weasel::Expression *weasel::Parser::parseExpressionOperator(unsigned precOrder, Expression *lhs)
{
    while (true)
    {
        auto binOp = getCurrentToken();
        if (!binOp.isOperator() || binOp.isNewline())
        {
            return lhs;
        }

        auto prec = binOp.getPrecedence();
        if (prec.order > precOrder)
        {
            return lhs;
        }

        getNextToken(); // eat 'operator'
        if (binOp.isOperatorCast())
        {
            auto castType = parseDataType();
            lhs = new TypeCastExpression(binOp, castType, lhs);

            continue;
        }

        auto rhs = parsePrimaryExpression();
        if (rhs->isError())
        {
            // Skip Expression till newline
            skipUntilNewLine();
        }
        else
        {
            rhs = parseExpressionOperator(prec.order, rhs);
        }

        lhs = createOperatorExpression(binOp, lhs, rhs);
    }
}

weasel::Expression *weasel::Parser::parseFieldExpression(Expression *lhs)
{
    LOG(INFO) << "Parse Field Expression of " << lhs->getToken().getValue();

    auto token = getCurrentToken();
    auto identToken = getNextToken();
    auto expr = new FieldExpression(token);
    if (!identToken.isIdentifier())
    {
        skipUntilNewLine();

        expr->setError(Errors::getInstance().expectedIdentifier);

        return expr;
    }

    if (expectToken(TokenKind::TokenDelimOpenParen))
    {
        return parseMethodCallExpression(lhs);
    }

    expr->setField(getCurrentToken().getValue());
    expr->setParentField(lhs);

    getNextToken(); // eat 'identifier'

    return expr;
}

weasel::Expression *weasel::Parser::parseReturnExpression()
{
    auto retToken = getCurrentToken();

    if (getNextToken().isNewline())
    {
        return new ReturnExpression(retToken, nullptr, weasel::Type::getVoidType());
    }

    return new ReturnExpression(retToken, parseExpression());
}

weasel::Expression *weasel::Parser::parseBreakExpression()
{
    auto token = getCurrentToken();
    if (getNextToken().isOpenParen())
    {
        return new BreakExpression(token, parseExpression());
    }

    return new BreakExpression(token, nullptr);
}

weasel::Expression *weasel::Parser::parseContinueExpression()
{
    auto token = getCurrentToken();
    if (getNextToken().isOpenParen())
    {
        return new ContinueExpression(token, parseExpression());
    }

    return new ContinueExpression(token, nullptr);
}
