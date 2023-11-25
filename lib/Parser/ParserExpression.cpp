#include <iostream>

#include "weasel/Parser/Parser.h"
#include "weasel/Symbol/Symbol.h"

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

    return new MethodCallExpression(callToken, implExpression, fun, args);
}

weasel::Expression *weasel::Parser::parseStaticMethodCallExpression(StructType *structType)
{
    getNextToken(); // eat last Struct

    // eat '.' and check next token
    auto callToken = getNextToken();

    // Find Functin Within Struct
    auto fun = findFunction(callToken.getValue(), structType, true);
    return parseCallExpression(fun);
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

    return new CallExpression(callToken, fun, args);
}

weasel::Expression *weasel::Parser::parseIdentifierExpression()
{
    // Check Available Function
    auto identToken = getCurrentToken();
    auto identifier = identToken.getValue();
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
    if (attr.getValue()->isArrayType())
    {
        if (getCurrentToken().isOpenSquare())
        {
            getNextToken(); // eat [
            auto indexExpr = parseExpression();
            if (!getCurrentToken().isCloseSquare())
            {
                return ErrorTable::addError(getCurrentToken(), "Expected ']'");
            }

            getNextToken(); // eat ]
            return new ArrayExpression(indexExpr->getToken(), identifier, indexExpr, attr.getValue()->getContainedType());
        }
    }

    auto type = attr.getValue();
    return new VariableExpression(identToken, identifier, type);
    // auto newVariable = new VariableExpression(identToken, identifier, type);
    // if (!type->isReferenceType())
    // {
    //     return newVariable;
    // }

    // return new UnaryExpression(identToken, UnaryExpression::Dereference, newVariable);
}

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

        auto colonToken = getNextToken(true);
        if (!colonToken.isColon())
        {
            return ErrorTable::addError(colonToken, "Expected Colon");
        }

        auto exprToken = getNextToken(true); // eat ':'
        auto expr = parseExpression();
        if (expr == nullptr)
        {
            return ErrorTable::addError(exprToken, "Expected Expression");
        }

        auto field = new StructExpression::StructField(idenToken.getValue(), expr);
        fields.push_back(field);

        ignoreNewline();

        if (getCurrentToken().isCloseCurly())
        {
            break;
        }

        if (!getCurrentToken().isComma())
        {
            return ErrorTable::addError(exprToken, "Expected Comma");
        }

        getNextToken(true); // eat ','
    }

    getNextToken(); // eat '}'

    return new StructExpression(token, userType, fields);
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
            auto userType = findUserType(getCurrentToken().getValue());
            if (userType != nullptr)
            {
                if (this->expectToken(TokenKind::TokenPuncDot))
                {
                    expr = parseStaticMethodCallExpression(userType);
                }
                else
                {
                    expr = parseStructExpression();
                }
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
            return parseFieldExpression(expr);
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
        if (expr == nullptr)
        {
            return ErrorTable::addError(getCurrentToken(), "Expected expression after unary operator");
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

    return ErrorTable::addError(getCurrentToken(), "Expected expression");
}

weasel::Expression *weasel::Parser::parseExpression()
{
    auto lhs = parsePrimaryExpression();
    if (lhs == nullptr)
    {
        return ErrorTable::addError(getCurrentToken(), "Expected RHS");
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
            if (!castType)
            {
                return ErrorTable::addError(getCurrentToken(), "Expected RHS Expression 1");
            }

            lhs = new TypeCastExpression(binOp, castType, lhs);

            continue;
        }

        auto rhs = parsePrimaryExpression();
        if (!rhs)
        {
            return ErrorTable::addError(getCurrentToken(), "Expected RHS Expression 1");
        }

        rhs = parseExpressionOperator(prec.order, rhs);
        if (!rhs)
        {
            return ErrorTable::addError(getCurrentToken(), "Expected RHS Expression 2");
        }

        lhs = createOperatorExpression(binOp, lhs, rhs);
    }
}

weasel::Expression *weasel::Parser::parseFieldExpression(Expression *lhs)
{
    LOG(INFO) << "Parse Field Expression of " << lhs->getToken().getValue();

    auto type = lhs->getType();
    auto token = getCurrentToken();
    auto identToken = getNextToken();
    if (!identToken.isIdentifier())
    {
        getNextTokenUntil(TokenKind::TokenSpaceNewline);

        return ErrorTable::addError(getCurrentToken(), "Expected Identifier for Field Expression");
    }

    if (!type->isPossibleStructType())
    {
        return ErrorTable::addError(lhs->getToken(), "Field " + identToken.getValue() + " isn't a struct");
    }

    if (expectToken(TokenKind::TokenDelimOpenParen))
    {
        return parseMethodCallExpression(lhs);
    }

    getNextToken(); // eat 'identifier'

    // TODO: Check on Analysis Semantic
    // Checking type field
    StructType *structType;
    if (type->isStructType())
    {
        structType = dynamic_cast<StructType *>(type);
    }
    else
    {
        structType = dynamic_cast<StructType *>(type->getContainedType());
    }
    auto idx = structType->findTypeName(identToken.getValue());
    if (idx == -1)
    {
        return ErrorTable::addError(getCurrentToken(), "Field " + identToken.getValue() + " not found");
    }

    auto typeField = structType->getContainedTypes()[idx];
    return new FieldExpression(token, identToken.getValue(), lhs, typeField);
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
