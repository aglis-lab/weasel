#include <iostream>
#include "weasel/Parser/Parser.h"
#include "weasel/Symbol/Symbol.h"

weasel::Expression *weasel::Parser::parseStatement()
{
    // Compound Statement Expression
    if (getCurrentToken().isKind(TokenKind::TokenDelimOpenCurlyBracket))
    {
        return parseCompoundStatement();
    }

    // If Statement
    if (getCurrentToken().isKind(TokenKind::TokenKeyIf))
    {
        return parseConditionStatement();
    }

    // For Statement
    if (getCurrentToken().isKeyFor())
    {
        return parseLoopingStatement();
    }

    // Variable Definition Expression
    if (getCurrentToken().isKeyDefinition())
    {
        return parseDeclarationExpression();
    }

    // Return Expression
    if (getCurrentToken().isKind(TokenKind::TokenKeyReturn))
    {
        return parseReturnExpression();
    }

    // Break Expression
    if (getCurrentToken().isKeyBreak())
    {
        return parseBreakExpression();
    }

    // Continue Expression
    if (getCurrentToken().isKeyContinue())
    {
        return parseContinueExpression();
    }

    auto expr = parseExpression();
    if (expr == nullptr)
    {
        auto errToken = getCurrentToken();

        getNextTokenUntil(TokenKind::TokenSpaceNewline);
        return ErrorTable::addError(errToken, "Invalid expression statement");
    }

    return expr;
}

weasel::Expression *weasel::Parser::parseLiteralExpression()
{
    auto token = getCurrentToken();
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

// TODO: Use Defined Function to validate the function call
// TODO: Eat last ')'
weasel::Expression *weasel::Parser::parseFunctionCallExpression(Function *fun)
{
    auto callToken = getCurrentToken();
    if (!getNextToken().isKind(TokenKind::TokenDelimOpenParen))
    {
        return ErrorTable::addError(getCurrentToken(), "Expected ( for function call");
    }

    std::vector<Expression *> args;
    if (!getNextToken().isKind(TokenKind::TokenDelimCloseParen))
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

            if (getCurrentToken().isKind(TokenKind::TokenDelimCloseParen))
            {
                break;
            }

            if (!getCurrentToken().isKind(TokenKind::TokenPuncComma))
            {
                return ErrorTable::addError(getCurrentToken(), "Expected ) or , in argument list");
            }

            getNextToken();
        }
    }

    return new MethodCallExpression(callToken, callToken.getValue(), args);
}

weasel::Expression *weasel::Parser::parseIdentifierExpression()
{
    auto identifier = getCurrentToken().getValue();

    // Check Available Function
    auto funExist = findFunction(identifier);
    if (funExist != nullptr)
    {
        return parseFunctionCallExpression(funExist);
    }

    // Check Variable
    auto attr = findAttribute(identifier);
    if (attr.isEmpty())
    {
        return ErrorTable::addError(getCurrentToken(), "Variable not yet declared");
    }

    // Check if Array Variable
    if (attr.getValue()->isArrayType() || attr.getValue()->isPointerType())
    {
        if (expectToken(TokenKind::TokenDelimOpenSquareBracket))
        {
            getNextToken(); // eat identifier
            getNextToken(); // eat [
            auto indexExpr = parseExpression();

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

weasel::Expression *weasel::Parser::parsePrimaryExpression()
{
    if (getCurrentToken().isLiteral())
    {
        return parseLiteralExpression();
    }

    if (getCurrentToken().isKind(TokenKind::TokenIdentifier))
    {
        return parseIdentifierExpression();
    }

    if (getCurrentToken().isKind(TokenKind::TokenDelimOpenParen))
    {
        return parseParenExpression();
    }

    if (getCurrentToken().isKind(TokenKind::TokenOperatorAnd))
    {
        if (getNextToken().isKind(TokenKind::TokenIdentifier))
        {
            return parseIdentifierExpression();
        }

        return ErrorTable::addError(getCurrentToken(), "Expected Variable Identifier for address of");
    }

    if (getCurrentToken().isKind(TokenKind::TokenDelimOpenSquareBracket))
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

    if (getNextToken().isKind(TokenKind::TokenSpaceNewline))
    {
        return lhs;
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

        lhs = new BinaryOperatorExpression(binOp, lhs, rhs);
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

weasel::Expression *weasel::Parser::parseDeclarationExpression()
{
    auto qualifier = getQualifier();
    auto qualToken = getCurrentToken();

    getNextToken(); // eat qualifier(let, final, const)
    if (!getCurrentToken().isKind(TokenKind::TokenIdentifier))
    {
        auto errToken = getCurrentToken();

        getNextTokenUntil(TokenKind::TokenSpaceNewline);
        return ErrorTable::addError(errToken, "Expected an identifier");
    }

    auto identifier = getCurrentToken().getValue();

    getNextToken(); // eat 'identifier' and get next token

    auto type = parseDataType();
    if (getCurrentToken().isKind(TokenKind::TokenSpaceNewline))
    {
        if (type == nullptr)
        {
            return ErrorTable::addError(getCurrentToken(), "Data Type Expected for default value declaration");
        }

        if (qualifier != Qualifier::QualVolatile)
        {
            return ErrorTable::addError(getCurrentToken(), "No Default Value for Non Volatile variable");
        }

        // Insert Symbol Table
        addAttribute(ParserAttribute::get(identifier, type, AttributeKind::Variable));

        // Create Variable with Default Value
        return new DeclarationExpression(qualToken, identifier, qualifier, type);
    }

    // Equal
    if (!getCurrentToken().isKind(TokenKind::TokenOperatorEqual))
    {
        auto errToken = getCurrentToken();

        getNextTokenUntil(TokenKind::TokenSpaceNewline);
        return ErrorTable::addError(errToken, "Expected equal sign");
    }

    // Get Next Value
    if (getNextToken().isKind(TokenKind::TokenSpaceNewline))
    {
        return ErrorTable::addError(getCurrentToken(), "Expected RHS Value Expression but got 'New line'");
    }

    auto val = parseExpression();
    if (!val)
    {
        auto errToken = getCurrentToken();

        getNextTokenUntil(TokenKind::TokenSpaceNewline);
        return ErrorTable::addError(errToken, "Expected RHS Value Expression but got not valid expression");
    }

    if (type == nullptr)
    {
        type = val->getType();
    }

    // Insert Symbol Table
    addAttribute(ParserAttribute::get(identifier, type, AttributeKind::Variable));

    return new DeclarationExpression(qualToken, identifier, qualifier, type, val);
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
