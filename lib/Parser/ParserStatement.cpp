#include "weasel/Parser/Parser.h"
#include "weasel/Symbol/Symbol.h"

weasel::Expression *weasel::Parser::parseLoopingStatement()
{
    auto token = getCurrentToken();
    auto isInfinity = getNextToken(true).isOpenCurly();
    auto conditions = std::vector<Expression *>();

    // Check if Infinity Looping
    if (!isInfinity)
    {
        // Initial or Definition
        if (!getCurrentToken().isSemiColon())
        {
            Expression *decl;
            if (getCurrentToken().isKeyDeclaration())
            {
                decl = parseDeclarationExpression();
            }
            else
            {
                decl = parseExpression();
            }

            conditions.push_back(decl);
        }

        // Break if looping is Loop Condition
        if (!getCurrentToken().isOpenCurly())
        {
            // Condition
            if (!getCurrentToken().isSemiColon())
            {
                return ErrorTable::addError(getCurrentToken(), "Invalid Loop condition expression");
            }
            if (getNextToken().isSemiColon())
            {
                conditions.push_back(nullptr);
            }
            else
            {
                conditions.push_back(parseExpression());
            }

            // Increment
            if (!getCurrentToken().isSemiColon())
            {
                return ErrorTable::addError(getCurrentToken(), "Invalid Loop counting expression");
            }
            if (getNextToken().isOpenCurly())
            {
                conditions.push_back(nullptr);
            }
            else
            {
                conditions.push_back(parseExpression());
            }
        }
    }

    if (!getCurrentToken().isOpenCurly())
    {
        return ErrorTable::addError(getCurrentToken(), "Invalid Loop body statement");
    }

    // Check if All Conditions is empty
    if (!isInfinity)
    {
        isInfinity = true;

        for (auto &item : conditions)
        {
            if (item != nullptr)
            {
                isInfinity = false;
                break;
            }
        }

        if (isInfinity)
        {
            conditions.clear();
        }
    }

    auto body = parseCompoundStatement();

    return new LoopingStatement(token, conditions, body);
}

weasel::Expression *weasel::Parser::parseConditionStatement()
{
    auto token = getCurrentToken();

    getNextToken(); // eat 'if'

    auto expr = parseExpression();
    if (expr == nullptr)
    {
        auto errToken = getCurrentToken();

        getNextTokenUntil(TokenKind::TokenSpaceNewline);
        return ErrorTable::addError(errToken, "Invalid condition expression");
    }

    auto body = parseCompoundStatement();
    if (body == nullptr)
    {
        auto errToken = getCurrentToken();

        getNextTokenUntil(TokenKind::TokenSpaceNewline);
        return ErrorTable::addError(errToken, "Invalid if body statement expression");
    }

    return new ConditionStatement(token, expr, body);
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
