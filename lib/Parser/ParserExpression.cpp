#include <iostream>

#include "weasel/Parser/Parser.h"
#include <weasel/Basic/Error.h>

GlobalVariableHandle Parser::parseGlobalVariable()
{
    LOG(INFO) << "Parse Global Variable";

    auto declStmt = parseDeclarationStatement();
    assert(declStmt);
    assert(!declStmt->isError());

    auto stmt = static_pointer_cast<DeclarationStatement>(declStmt);
    assert(!stmt->isError());

    auto expr = make_shared<GlobalVariable>();

    expr->setToken(stmt->getToken());
    expr->setType(stmt->getType());
    expr->setIdentifier(stmt->getIdentifier());
    expr->setQualifier(stmt->getQualifier());
    expr->setValue(stmt->getValue());
    if (stmt->isError())
    {
        expr->setError(stmt->getError().value());
        return expr;
    }

    return expr;
}

ExpressionHandle Parser::parseMethodCallExpression(ExpressionHandle lhs)
{
    LOG(INFO) << "Parse Method Call Expression";

    auto expr = make_shared<MethodCallExpression>();

    expr->setIdentifier(getCurrentToken().getValue());
    expr->setToken(getCurrentToken());
    expr->setLHS(lhs);

    getNextToken(); // eat 'identifier'
    return parseCallExpression(expr);
}

ExpressionHandle Parser::parseStaticMethodCallExpression()
{
    LOG(INFO) << "Parse Static Method Call Expression...";

    assert(false && "NOT IMPLEMENTED YET");

    return nullptr;

    // getNextToken(); // eat last Struct or identifier

    // // eat '.' and check next token
    // if (!getNextToken().isDot())
    // {
    //     return new ErrorExpression(getCurrentToken(), Errors::getInstance().expectedDot);
    // }

    // // Call Expression
    // return parseCallExpression();
}

ExpressionHandle Parser::parseUnaryExpression()
{
    LOG(INFO) << "Parse Unary Expression...";

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

    return make_shared<UnaryExpression>(token, op, expr);
}

ExpressionHandle Parser::parseLiteralExpression()
{
    LOG(INFO) << "Parse Literal Expression...";

    auto token = getCurrentToken();
    getNextToken(); // eat 'literal'

    if (token.isKind(TokenKind::TokenLitBool))
    {
        return make_shared<BoolLiteralExpression>(token, token.getValue() == "true");
    }

    if (token.isKind(TokenKind::TokenLitChar))
    {
        auto val = token.getValue()[0];
        return make_shared<CharLiteralExpression>(token, val);
    }

    if (token.isKind(TokenKind::TokenLitInteger))
    {
        auto value = strtoll(token.getValue().c_str(), nullptr, 10);
        return make_shared<NumberLiteralExpression>(token, value);
    }

    if (token.isKind(TokenKind::TokenLitFloat))
    {
        auto value = std::stof(token.getValue());
        return make_shared<FloatLiteralExpression>(token, value);
    }

    if (token.isKind(TokenKind::TokenLitDouble))
    {
        auto value = std::stod(token.getValue());
        return make_shared<DoubleLiteralExpression>(token, value);
    }

    if (token.isKind(TokenKind::TokenLitString))
    {
        auto *currentBuffer = token.getStartBuffer();
        auto *endBuffer = token.getEndBuffer();
        string value = "";

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

        return make_shared<StringLiteralExpression>(token, value);
    }

    return make_shared<NilLiteralExpression>(getCurrentToken());
}

tuple<vector<ExpressionHandle>, optional<Error>> Parser::parseArguments()
{
    vector<ExpressionHandle> args;

    if (!getNextToken().isCloseParen())
    {
        while (true)
        {
            auto arg = parseExpressionWithBlock();
            args.push_back(arg);
            if (arg->isError())
            {
                skipUntilNewLine();
                break;
            }

            if (getCurrentToken().isCloseParen())
            {
                break;
            }

            if (!getCurrentToken().isComma())
            {
                return {args, Errors::getInstance().expectedCloseParen.withToken(getCurrentToken())};
            }

            getNextToken(); // eat ','
        }
    }

    getNextToken(); // eat ')'

    return {args, {}};
}

ExpressionHandle Parser::parseCallExpression(ExpressionHandle lhs)
{
    LOG(INFO) << "Parse Call Expression...";

    auto expr = make_shared<CallExpression>(lhs->getToken());
    auto [args, err] = parseArguments();

    expr->setLHS(lhs);
    expr->setArguments(args);
    if (err)
    {
        expr->setError(err.value());
    }

    return expr;
}

ExpressionHandle Parser::parseIndexExpression(ExpressionHandle lhs)
{
    LOG(INFO) << "Parse Index Expression";

    auto expr = make_shared<IndexExpression>();

    expr->setLHS(lhs);

    getNextToken(); // eat [
    auto indexExpr = parseExpressionWithBlock();
    if (!getCurrentToken().isCloseSquare())
    {
        return make_shared<ErrorExpression>(indexExpr->getToken(), Errors::getInstance().expectedCloseSquare.withToken(indexExpr->getToken()));
    }

    getNextToken(); // eat ]
    expr->setToken(indexExpr->getToken());
    expr->setIndexExpression(indexExpr);

    return expr;
}

ExpressionHandle Parser::parseIdentifierExpression()
{
    LOG(INFO) << "Parse Identifier Expression";

    auto identToken = getCurrentToken();
    getNextToken(); // eat 'identifier'
    return make_shared<VariableExpression>(identToken, identToken.getValue());
}

ExpressionHandle Parser::parseParenExpression()
{
    LOG(INFO) << "Parse paren Expression...";

    getNextToken(); // eat '('
    auto expr = parseExpressionWithBlock();
    if (!getCurrentToken().isCloseParen())
    {
        expr->setError(Errors::getInstance().expectedOpenParen.withToken(getCurrentToken()));
        return expr;
    }

    getNextToken(); // eat ')'
    return expr;
}

// Default Type
ExpressionHandle Parser::parseArrayExpression()
{
    LOG(INFO) << "Parsing Array\n";

    auto expr = make_shared<ArrayExpression>();

    getNextToken(); // eat [
    while (!getCurrentToken().isKind(TokenKind::TokenDelimCloseSquareBracket))
    {
        expr->getItems().push_back(parseLiteralExpression());
        if (getCurrentToken().isKind(TokenKind::TokenPuncComma))
        {
            getNextToken(); // eat ,
        }
    }

    getNextToken(); // eat ]
    return expr;
}

ExpressionHandle Parser::parseStructExpression(VariableExpressionHandle lhs)
{
    LOG(INFO) << "Parse struct Expression";

    assert(getCurrentToken().isOpenCurly());

    auto expr = make_shared<StructExpression>(lhs->getToken(), lhs->getIdentifier());

    getNextToken(true); // eat '{'
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
        auto valueExpr = parseExpressionWithBlock();
        auto field = make_shared<StructExpression::StructField>(idenToken.getValue(), valueExpr);
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

ExpressionHandle Parser::parsePrimaryExpression()
{
    LOG(INFO) << "Parse Primary Expression";

    ExpressionHandle lhs;

    // Literal Expression
    if (getCurrentToken().isLiteral())
    {
        lhs = parseLiteralExpression();
    }
    // Parse Indentifier Like Expression
    else if (getCurrentToken().isIdentifier())
    {
        lhs = parseIdentifierExpression();
    }
    // Array Expression
    else if (getCurrentToken().isOpenSquare())
    {
        lhs = parseArrayExpression();
    }
    // Unary Expression
    else if (getCurrentToken().isOperatorUnary())
    {
        lhs = parseUnaryExpression();
    }
    // Open Paren Expression
    else if (getCurrentToken().isOpenParen())
    {
        lhs = parseParenExpression();
    }
    // Parse Lambda
    else if (getCurrentToken().isKeyFunction())
    {
        lhs = parseLambdaExpression();
    }

    if (!lhs)
    {
        return make_shared<ErrorExpression>(getCurrentToken(), Errors::getInstance().expectedExpression.withToken(getCurrentToken()));
    }

    if (lhs->isError())
    {
        return lhs;
    }

    // Parse Multi Expression
    while (true)
    {
        // Call Expression
        if (getCurrentToken().isOpenParen())
        {
            lhs = parseCallExpression(lhs);
            continue;
        }

        // Field Expression
        if (getCurrentToken().isDot())
        {
            lhs = parseFieldExpression(lhs);
            continue;
        }

        // Index Expression
        if (getCurrentToken().isOpenSquare())
        {
            lhs = parseIndexExpression(lhs);
            continue;
        }

        break;
    }

    return lhs;
}

ExpressionHandle Parser::parseExpressionWithBlock()
{
    LOG(INFO) << "Parse Expression";

    auto lhs = parsePrimaryExpression();
    if (lhs->isError())
    {
        skipUntilNewLine();
        return lhs;
    }

    if (lhs->isVariableExpression() && getCurrentToken().isOpenCurly())
    {
        lhs = parseStructExpression(static_pointer_cast<VariableExpression>(lhs));
    }
    if (lhs->isError())
    {
        skipUntilNewLine();
        return lhs;
    }

    return parseBinaryExpression(__defaultPrecOrder, lhs);
}

ExpressionHandle Parser::parseExpressionWithoutBlock()
{
    LOG(INFO) << "Parse Condition Expression...";

    auto lhs = parsePrimaryExpression();
    if (lhs->isError())
    {
        skipUntilNewLine();
        return lhs;
    }

    return parseBinaryExpression(__defaultPrecOrder, lhs);
}

ExpressionHandle Parser::parseBinaryExpression(unsigned precOrder, ExpressionHandle lhs)
{
    LOG(INFO) << "Parse Expression Operator";

    while (true)
    {
        auto binOp = getCurrentToken();

        if (!binOp.isOperator())
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
            lhs = make_shared<TypeCastExpression>(binOp, castType, lhs);

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
            rhs = parseBinaryExpression(prec.order, rhs);
        }

        lhs = createOperatorExpression(binOp, lhs, rhs);
        if (rhs->isError())
        {
            return lhs;
        }
    }

    return make_shared<ErrorExpression>(getCurrentToken(), Errors::getInstance().unimplementedSyntax.withToken(getCurrentToken()));
}

ExpressionHandle Parser::parseFieldExpression(ExpressionHandle lhs)
{
    LOG(INFO) << "Parse Field Expression of " << lhs->getToken().getValue();

    getNextToken(); // eat '.'
    if (expectToken().isOpenParen())
    {
        return parseMethodCallExpression(lhs);
    }

    auto expr = make_shared<FieldExpression>(getCurrentToken(), getCurrentToken().getValue());
    expr->setLHS(lhs);

    getNextToken(); // eat 'identifier'

    return expr;
}

ExpressionHandle Parser::parseReturnExpression()
{
    LOG(INFO) << "Parse Return Expression...";

    auto retToken = getCurrentToken();
    if (getNextToken().isNewline())
    {
        return make_shared<ReturnExpression>(retToken, Type::getVoidType());
    }

    auto exprValue = parseExpressionWithBlock();
    return make_shared<ReturnExpression>(retToken, exprValue);
}

ExpressionHandle Parser::parseBreakExpression()
{
    LOG(INFO) << "Parse Break Expression...";

    auto token = getCurrentToken();
    auto expr = make_shared<BreakExpression>(token, nullptr);
    if (getNextToken().isOpenParen())
    {
        expr->setValue(parseExpressionWithBlock());
    }

    return expr;
}

ExpressionHandle Parser::parseContinueExpression()
{
    LOG(INFO) << "Parse Continue Expression...";

    auto token = getCurrentToken();
    if (getNextToken().isOpenParen())
    {
        return make_shared<ContinueExpression>(token, parseExpressionWithBlock());
    }

    return make_shared<ContinueExpression>(token, nullptr);
}
