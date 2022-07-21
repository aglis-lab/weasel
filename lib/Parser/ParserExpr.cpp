#include <iostream>
#include "weasel/Parser/Parser.h"
#include "weasel/IR/Context.h"
#include "weasel/Symbol/Symbol.h"

weasel::Expression *weasel::Parser::parseStatement()
{
    // Compound Statement Expression
    if (getCurrentToken().isKind(TokenKind::TokenDelimOpenCurlyBracket))
    {
        return parseCompoundStatement();
    }

    // Variable Definition Expression
    if (getCurrentToken().isKeyDefinition())
    {
        return parseDeclarationExpression();
    }

    // Return Expression
    if (getCurrentToken().isKind(TokenKind::TokenKeyReturn))
    {
        return parseReturnStatement();
    }

    // If Statement
    if (getCurrentToken().isKind(TokenKind::TokenKeyIf))
    {
        return parseIfStatement();
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

weasel::Expression *weasel::Parser::parseIfStatement()
{
    auto token = getCurrentToken();

    getNextToken(); // eat 'if'

    auto expr = parsePrimaryExpression();
    if (expr == nullptr)
    {
        auto errToken = getCurrentToken();

        getNextTokenUntil(TokenKind::TokenSpaceNewline);
        ErrorTable::addError(errToken, "Invalid condition expression");
    }

    getNextToken(true); // eat ')'

    auto body = parseCompoundStatement();
    if (body == nullptr)
    {
        auto errToken = getCurrentToken();

        getNextTokenUntil(TokenKind::TokenSpaceNewline);
        ErrorTable::addError(errToken, "Invalid if body statement expression");
    }

    return new ConditionStatementExpression(token, expr, body);
}

weasel::StatementExpression *weasel::Parser::parseCompoundStatement()
{
    if (!getCurrentToken().isKind(TokenKind::TokenDelimOpenCurlyBracket))
    {
        return nullptr;
    }

    auto stmt = new StatementExpression();
    if (getNextToken(true).isKind(TokenKind::TokenDelimCloseCurlyBracket))
    {
        getNextToken();
        return stmt;
    }

    // Enter statement scope
    {
        SymbolTable::enterScope();
    }

    do
    {
        auto expr = parseStatement();
        if (expr != nullptr)
        {
            stmt->addBody(expr);
        }
        else
        {
            ErrorTable::addError(getCurrentToken(), "Expected statement");
        }

        if (getCurrentToken().isKind(TokenKind::TokenDelimCloseCurlyBracket))
        {
            break;
        }

        if (expr->isCompoundExpression() && !getCurrentToken().isNewline())
        {
            continue;
        }

        if (!getCurrentToken().isNewline())
        {
            ErrorTable::addError(getCurrentToken(), "Expected New Line");
        }

        getNextToken(true);
    } while (!getCurrentToken().isKind(TokenKind::TokenDelimCloseCurlyBracket));

    getNextToken(); // eat '}'

    // Exit statement scope
    {
        SymbolTable::exitScope();
    }

    return stmt;
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

weasel::Expression *weasel::Parser::parseFunctionCallExpression(weasel::Attribute *attr)
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

    // TODO: We should eat ')'
    std::cout << "Function Call: '" << callToken.getValue() << "' with current token ";
    std::cout << getCurrentToken().getValue() << std::endl;

    return new CallExpression(callToken, callToken.getValue(), args);
}

weasel::Expression *weasel::Parser::parseIdentifierExpression()
{
    auto identifier = getCurrentToken().getValue();
    auto attr = SymbolTable::get(identifier);
    if (!attr)
    {
        return ErrorTable::addError(getCurrentToken(), "Variable not yet declared");
    }

    if (attr->getKind() == AttributeKind::SymbolFunction)
    {
        return parseFunctionCallExpression(attr);
    }

    // Check if Array Variable
    if (attr->isKind(AttributeKind::SymbolArray) || attr->isKind(AttributeKind::SymbolPointer))
    {
        if (expectToken(TokenKind::TokenDelimOpenSquareBracket))
        {
            getNextToken(); // eat identifier
            getNextToken(); // eat [
            auto indexExpr = parseExpression();

            return new ArrayExpression(indexExpr->getToken(), identifier, indexExpr, attr->getType());
        }
    }

    return new VariableExpression(getCurrentToken(), identifier, attr->getType());
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

weasel::Expression *weasel::Parser::parseReturnStatement()
{
    auto retToken = getCurrentToken();
    getNextToken(); // eat 'return'

    auto expr = parseExpression();
    if (!expr)
    {
        auto errToken = getCurrentToken();

        getNextTokenUntil(TokenKind::TokenSpaceNewline);
        return ErrorTable::addError(errToken, "Expected expression for return statement.");
    }

    return new ReturnExpression(retToken, expr);
}

// let 'identifier' 'datatype'  = 'expr'
// let 'identifier' 'datatype'
// let 'identifier'             = 'expr'
// let 'identifier' *'datatype'
// let 'identifier'             = &'expr'
// let 'identifier' [<size>]'datatype'
// let 'identifier'             = []
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
        {
            AttributeKind attrKind;
            if (type->isArrayType())
            {
                attrKind = AttributeKind::SymbolArray;
            }
            else if (type->isPointerType())
            {
                attrKind = AttributeKind::SymbolPointer;
            }
            else
            {
                attrKind = AttributeKind::SymbolVariable;
            }

            auto attr = new Attribute(identifier, AttributeScope::ScopeLocal, attrKind, type);
            SymbolTable::insert(identifier, attr);
        }

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

    // Get Value
    getNextToken(); // eat 'Equal Sign'
    if (getCurrentToken().isKind(TokenKind::TokenSpaceNewline))
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
    {
        AttributeKind attrKind = AttributeKind::SymbolVariable;
        if (type->isArrayType())
        {
            attrKind = AttributeKind::SymbolArray;
        }
        else if (type->isPointerType())
        {
            attrKind = AttributeKind::SymbolPointer;
        }

        auto attr = new Attribute(identifier, AttributeScope::ScopeLocal, attrKind, type);
        SymbolTable::insert(identifier, attr);
    }

    return new DeclarationExpression(qualToken, identifier, qualifier, type, val);
}
