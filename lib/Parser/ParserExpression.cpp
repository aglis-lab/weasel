#include <iostream>

#include "weasel/Parser/Parser.h"
#include <weasel/Basic/Error.h>

GlobalVariableHandle Parser::parseGlobalVariable()
{
    LOG(INFO) << "Parse Global Variable...";

    return nullptr;
    // auto stmt = static_pointer_cast<DeclarationStatement>(parseDeclarationExpression());
    // if (!stmt->isError() && stmt->getQualifier() != Qualifier::QualConst)
    // {
    //     return stmt;
    // }

    // return stmt;
}

// Expression *Parser::parseMethodCallExpression(Expression *implExpression)
// {
//     return nullptr;

//     // auto callToken = getCurrentToken();
//     // auto expr = new MethodCallExpression(callToken);

//     // if (!getNextToken().isOpenParen())
//     // {
//     //     expr->setError(Errors::getInstance().expectedOpenParen);

//     //     return expr;
//     // }

//     // if (!getNextToken().isCloseParen())
//     // {
//     //     while (true)
//     //     {
//     //         auto arg = parseExpression();
//     //         expr->getArguments().push_back(arg);

//     //         if (getCurrentToken().isCloseParen())
//     //         {
//     //             break;
//     //         }

//     //         if (!getCurrentToken().isComma())
//     //         {
//     //             expr->setError(Errors::getInstance().expectedCloseParen);
//     //             return expr;
//     //         }

//     //         getNextToken();
//     //     }
//     // }

//     // getNextToken(); // eat ')'

//     // StructType *structType;
//     // if (implExpression->getType()->isStructType())
//     // {
//     //     structType = dynamic_cast<StructType *>(implExpression->getType());
//     // }
//     // else
//     // {
//     //     structType = dynamic_cast<StructType *>(implExpression->getType()->getContainedType());
//     // }
//     // auto fun = findFunction(callToken.getValue(), structType);

//     // expr->setType(fun->getType());
//     // expr->setImplExpression(implExpression);
//     // expr->setFunction(fun);

//     // return expr;
// }

ExpressionHandle Parser::parseStaticMethodCallExpression()
{
    LOG(INFO) << "Parse Static Method Call Expression...";

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

ExpressionHandle Parser::parseCallExpression()
{
    LOG(INFO) << "Parse Call Expression...";

    auto callToken = getCurrentToken();
    auto expr = make_shared<CallExpression>(callToken, callToken.getValue());
    if (!getNextToken().isOpenParen())
    {
        expr->setError(Errors::getInstance().expectedOpenParen.withToken(getCurrentToken()));
        return expr;
    }

    if (!getNextToken().isCloseParen())
    {
        while (true)
        {
            auto arg = parseExpression();
            expr->getArguments().push_back(arg);
            if (arg->isError())
            {
                skipUntilNewLine();
                return expr;
            }

            if (getCurrentToken().isCloseParen())
            {
                break;
            }

            if (!getCurrentToken().isComma())
            {
                expr->setError(Errors::getInstance().expectedCloseParen.withToken(getCurrentToken()));
                return expr;
            }

            getNextToken(); // eat ','
        }
    }

    getNextToken(); // eat ')'

    return expr;
}

ExpressionHandle Parser::parseIdentifierExpression()
{
    LOG(INFO) << "Parse Identifier Expression...";

    // Open Paren Expression
    if (getCurrentToken().isOpenParen())
    {
        return parseParenExpression();
    }

    // Check Available Function
    if (expectToken().isOpenParen())
    {
        return parseCallExpression();
    }

    // TODO: Access Array Expression
    // if (getCurrentToken().isOpenSquare())
    // {
    //     getNextToken(); // eat [
    //     auto indexExpr = parseExpression();
    //     if (!getCurrentToken().isCloseSquare())
    //     {
    //         return new ErrorExpression(indexExpr->getToken(), Errors::getInstance().expectedCloseSquare);
    //     }

    //     getNextToken(); // eat ]
    //     return new ArrayExpression(indexExpr->getToken(), identifier, indexExpr);
    // }

    auto identToken = getCurrentToken();
    getNextToken(); // eat 'identifier'

    return make_shared<VariableExpression>(identToken, identToken.getValue());
}

ExpressionHandle Parser::parseParenExpression()
{
    LOG(INFO) << "Parse paren Expression...";

    getNextToken(); // eat '('
    auto expr = parseExpression();
    if (!getCurrentToken().isCloseParen())
    {
        expr->setError(Errors::getInstance().expectedOpenParen.withToken(getCurrentToken()));
        return expr;
    }

    getNextToken(); // eat ')'
    return expr;
}

// Default Type
// Expression *Parser::parseArrayExpression()
// {
//     LOG(INFO) << "Parsing Array\n";

//     std::vector<Expression *> items;

//     getNextToken(); // eat [
//     while (!getCurrentToken().isKind(TokenKind::TokenDelimCloseSquareBracket))
//     {
//         items.push_back(parseLiteralExpression());
//         if (getCurrentToken().isKind(TokenKind::TokenPuncComma))
//         {
//             getNextToken(); // eat ,
//         }
//     }

//     getNextToken(); // eat ]
//     return new ArrayLiteralExpression(items);
// }

ExpressionHandle Parser::parseStructExpression()
{
    LOG(INFO) << "Parse struct Expression...";

    auto expr = make_shared<StructExpression>(getCurrentToken(), getCurrentToken().getValue());

    // Eat Identifier and next to '{'
    if (!getNextToken().isOpenCurly())
    {
        expr->setError(Errors::getInstance().expectedOpenCurly.withToken(getCurrentToken()));
        return expr;
    }

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
        auto valueExpr = parseExpression();
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

    // Parse Struct Expression
    if (getCurrentToken().isIdentifier() && expectToken().isOpenCurly())
    {
        return parseStructExpression();
    }

    // Parse Lambda
    if (getCurrentToken().isKeyFunction())
    {
        return parseLambdaExpression();
    }

    // Parse Indentifier Like Expression
    if (getCurrentToken().isIdentifier() || getCurrentToken().isOpenParen())
    {
        auto expr = parseIdentifierExpression();
        if (expr->isError())
        {
            return expr;
        }

        while (getCurrentToken().isDot())
        {
            expr = parseFieldExpression(expr);
            if (expr->isError())
            {
                break;
            }
        }

        return expr;
    }

    // if (getCurrentToken().isOpenSquare())
    // {
    //     return parseArrayExpression();
    // }

    // if (possibleHaveField)
    // {
    //     ExpressionHandle expr = nullptr;

    //     // Identifier
    //     // Call or Variable Expression or Struct Expression
    //     if (getCurrentToken().isIdentifier() || getCurrentToken().isKeyThis())
    //     {
    //         if (expectToken(TokenKind::TokenDelimOpenCurlyBracket))
    //         {
    //             expr = parseStructExpression();
    //         }
    //         else
    //         {
    //             expr = parseIdentifierExpression();
    //         }

    //         // if (expectToken(TokenKind::TokenPuncDot))
    //         // {
    //         //     expr = parseStaticMethodCallExpression();
    //         // }
    //         // else if (expectToken(TokenKind::TokenDelimOpenCurlyBracket))
    //         // {
    //         //     expr = parseStructExpression();
    //         // }
    //         // else
    //         // {
    //         //     expr = parseIdentifierExpression();
    //         // }
    //     }

    //     // Parentise Expression
    //     else if (getCurrentToken().isOpenParen())
    //     {
    //         expr = parseParenExpression();
    //     }

    //     //     // Array Expression
    //     //     else if (getCurrentToken().isOpenSquare())
    //     //     {
    //     //         expr = parseArrayExpression();
    //     //     }

    //     // Check for possible Field Expression
    //     if (expr != nullptr && getCurrentToken().isDot())
    //     {
    //         expr = parseFieldExpression(expr);
    //     }

    //     return expr;
    // }

    // Literal Expression
    if (getCurrentToken().isLiteral())
    {
        return parseLiteralExpression();
    }

    // Unary Expression
    if (getCurrentToken().isOperatorUnary())
    {
        return parseUnaryExpression();
    }

    return make_shared<ErrorExpression>(getCurrentToken(), Errors::getInstance().expectedExpression.withToken(getCurrentToken()));
}

ExpressionHandle Parser::parseExpression()
{
    LOG(INFO) << "Parse Expression...";

    auto lhs = parsePrimaryExpression();
    if (lhs->isError())
    {
        skipUntilNewLine();
        return lhs;
    }

    return parseExpressionOperator(__defaultPrecOrder, lhs);
}

ExpressionHandle Parser::parseExpressionOperator(unsigned precOrder, ExpressionHandle lhs)
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
            rhs = parseExpressionOperator(prec.order, rhs);
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

    auto identToken = getNextToken();
    auto expr = make_shared<FieldExpression>(identToken, identToken.getValue());
    if (!identToken.isIdentifier())
    {
        skipUntilNewLine();

        expr->setError(Errors::getInstance().expectedIdentifier);

        return expr;
    }

    // if (expectToken(TokenKind::TokenDelimOpenParen))
    // {
    //     return parseMethodCallExpression(lhs);
    // }

    expr->setIdentifier(getCurrentToken().getValue());
    expr->setParentField(lhs);

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

    auto exprValue = parseExpression();
    return make_shared<ReturnExpression>(retToken, exprValue);
}

ExpressionHandle Parser::parseBreakExpression()
{
    LOG(INFO) << "Parse Break Expression...";

    auto token = getCurrentToken();
    auto expr = make_shared<BreakExpression>(token, nullptr);
    if (getNextToken().isOpenParen())
    {
        expr->setValue(parseExpression());
    }

    return expr;
}

ExpressionHandle Parser::parseContinueExpression()
{
    LOG(INFO) << "Parse Continue Expression...";

    auto token = getCurrentToken();
    if (getNextToken().isOpenParen())
    {
        return make_shared<ContinueExpression>(token, parseExpression());
    }

    return make_shared<ContinueExpression>(token, nullptr);
}
