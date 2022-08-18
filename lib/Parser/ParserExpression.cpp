#include <iostream>
#include "weasel/Parser/Parser.h"
#include "weasel/Symbol/Symbol.h"

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

weasel::Expression *weasel::Parser::parseCallExpression(Function *fun)
{
    auto callToken = getCurrentToken();
    if (!getNextToken().isOpenParen())
    {
        return ErrorTable::addError(getCurrentToken(), "Expected ( for function call");
    }

    std::vector<Expression *> args;
    if (!getNextToken().isCloseParen())
    {
        while (true)
        {
            if (auto arg = parseExpression())
            {
                args.push_back(arg);
            }
            else
            {
                return ErrorTable::addError(getCurrentToken(), "Expected argument expression");
            }

            if (getCurrentToken().isCloseParen())
            {
                break;
            }

            if (!getCurrentToken().isComma())
            {
                return ErrorTable::addError(getCurrentToken(), "Expected ) or , in argument list");
            }

            getNextToken();
        }
    }

    getNextToken(); // eat ')'

    return new CallExpression(callToken, callToken.getValue(), args);
}

weasel::Expression *weasel::Parser::parseIdentifierExpression()
{
    // Check Available Function
    auto identifier = getCurrentToken().getValue();
    auto funExist = findFunction(identifier);
    if (funExist != nullptr)
    {
        return parseCallExpression(funExist);
    }

    // Check Variable
    auto attr = findAttribute(identifier);
    if (attr.isEmpty())
    {
        return ErrorTable::addError(getCurrentToken(), "Variable not yet declared");
    }

    getNextToken(); // eat identifier

    // Check if Array Variable
    if (attr.getValue()->isArrayType() || attr.getValue()->isPointerType())
    {
        if (getCurrentToken().isOpenSquare())
        {
            getNextToken(); // eat [
            auto indexExpr = parseExpression();
            getNextToken(); // eat ]

            return new ArrayExpression(indexExpr->getToken(), identifier, indexExpr, attr.getValue());
        }
    }

    return new VariableExpression(getCurrentToken(), identifier, attr.getValue());
}

// TODO: Need to consume last ')'
weasel::Expression *weasel::Parser::parseParenExpression()
{
    getNextToken(); // eat (
    auto expr = parseExpression();
    if (!expr)
    {
        return ErrorTable::addError(getCurrentToken(), "Expected expression inside after (..");
    }

    if (!getCurrentToken().isCloseParen())
    {
        return ErrorTable::addError(getCurrentToken(), "Expected )");
    }

    // Token ) will eated next time
    return expr;
}

weasel::Expression *weasel::Parser::parseArrayExpression()
{
    auto expr = new ArrayLiteralExpression();

    getNextToken(); // eat [
    while (!getCurrentToken().isKind(TokenKind::TokenDelimCloseSquareBracket))
    {
        expr->addItem(parseLiteralExpression());

        if (getNextToken().isKind(TokenKind::TokenPuncComma))
        {
            getNextToken();
        }
    }

    return expr;
}

weasel::Expression *weasel::Parser::parseStructExpression()
{
    auto token = getCurrentToken();
    auto userType = findUserType(token.getValue());
    if (!getNextToken(true).isOpenCurly())
    {
        auto token = getCurrentToken();
        getNextTokenUntil(TokenKind::TokenSpaceNewline);
        return ErrorTable::addError(token, "Expected { after struct");
    }

    getNextToken(true); // eat '{'
    std::vector<StructExpression::StructField *> fields;
    while (!getCurrentToken().isCloseCurly())
    {
        auto idenToken = getCurrentToken();
        if (!idenToken.isIdentifier())
        {
            return ErrorTable::addError(idenToken, "Expected Identifier");
        }

        auto colonToken = getNextToken();
        if (!colonToken.isColon())
        {
            return ErrorTable::addError(colonToken, "Expected Colon");
        }

        auto exprToken = getNextToken(); // eat ':'
        auto expr = parseExpression();
        if (expr == nullptr)
        {
            return ErrorTable::addError(exprToken, "Expected Expression");
        }

        auto field = new StructExpression::StructField(idenToken.getValue(), expr);
        fields.push_back(field);

        if (getCurrentToken().isCloseCurly())
        {
            break;
        }

        if (!getCurrentToken().isComma())
        {
            return ErrorTable::addError(exprToken, "Expected Comma");
        }

        getNextToken(); // eat ','
    }

    getNextToken(); // eat '}'

    return new StructExpression(token, userType, fields);
}

weasel::Expression *weasel::Parser::parsePrimaryExpression()
{
    if (getCurrentToken().isLiteral())
    {
        return parseLiteralExpression();
    }

    // Struct Expression
    if (getCurrentToken().isIdentifier())
    {
        auto userType = findUserType(getCurrentToken().getValue());
        if (userType != nullptr)
        {
            return parseStructExpression();
        }
    }

    if (getCurrentToken().isIdentifier())
    {
        return parseIdentifierExpression();
    }

    if (getCurrentToken().isOpenParen())
    {
        return parseParenExpression();
    }

    // Unary Expression
    if (getCurrentToken().isOperatorUnary())
    {
        auto token = getCurrentToken();

        getNextToken(); // eat '& | * | - | !'

        auto expr = parsePrimaryExpression();
        if (expr == nullptr)
        {
            return ErrorTable::addError(getCurrentToken(), "Expected expression after address of");
        }

        auto op = UnaryExpression::Borrow;
        switch (token.getTokenKind())
        {
        case TokenKind::TokenOperatorStar:
            op = UnaryExpression::Dereference;
            break;
        case TokenKind::TokenOperatorNegative:
            op = UnaryExpression::Negative;
            break;
        case TokenKind::TokenOperatorNot:
            op = UnaryExpression::Not;
            break;
        }

        return new UnaryExpression(token, op, expr);
    }

    // Array Expression
    if (getCurrentToken().isOpenSquare())
    {
        return parseArrayExpression();
    }

    return ErrorTable::addError(getCurrentToken(), "Expected expression");
}

weasel::Expression *weasel::Parser::parseExpression()
{
    auto lhs = parsePrimaryExpression();
    if (lhs == nullptr)
    {
        return ErrorTable::addError(getCurrentToken(), "Expected LHS");
    }

    return parseBinaryOperator(__defaultPrecOrder, lhs);
}

weasel::Expression *weasel::Parser::parseBinaryOperator(unsigned precOrder, Expression *lhs)
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
        auto rhs = parsePrimaryExpression();
        if (!rhs)
        {
            return ErrorTable::addError(getCurrentToken(), "Expected RHS Expression 1");
        }

        getNextToken(); // eat 'rhs'
        rhs = parseBinaryOperator(prec.order, rhs);
        if (!rhs)
        {
            return ErrorTable::addError(getCurrentToken(), "Expected RHS Expression 2");
        }

        lhs = new BinaryExpression(binOp, lhs, rhs);
    }
}

weasel::Expression *weasel::Parser::parseReturnExpression()
{
    auto retToken = getCurrentToken();

    if (getNextToken().isNewline())
    {
        return new ReturnExpression(retToken, nullptr);
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
