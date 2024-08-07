#include "weasel/Parser/Parser.h"

// parse
void weasel::Parser::parse()
{
    while (!getNextToken().isEnd())
    {
        while (getCurrentToken().isNewline())
        {
            getNextToken();
        }

        if (getCurrentToken().isEnd())
        {
            return;
        }

        if (getCurrentToken().isKeyStruct())
        {
            auto structType = parseStruct();
            getModule()->addUserType(structType);
        }

        if (getCurrentToken().isKeyFunction() || getCurrentToken().isKeyExtern())
        {
            auto isExtern = false;
            if (getCurrentToken().isKeyExtern())
            {
                isExtern = true;
                getNextToken(); // eat 'extern'
            }

            auto fun = parseFunction();
            fun->setIsExtern(isExtern);
            getModule()->addFunction(fun);
        }

        if (getCurrentToken().isKeyDeclaration())
        {
            getModule()->addGlobalVariable(parseGlobalVariable());
        }
    }
}

// Get Next Token Until
weasel::Token weasel::Parser::getNextTokenUntil(weasel::TokenKind kind)
{
    if (getCurrentToken().isKind(kind))
    {
        return getCurrentToken();
    }

    while (true)
    {
        auto token = getNextToken();
        if (token.isKind(kind))
        {
            return token;
        }

        if (token.isKind(TokenKind::TokenEOF) || token.isUnknown())
        {
            break;
        }
    }

    return Token::empty();
}

// Get Next Token
Token Parser::getNextToken(bool skipSpace)
{
    return _lexer.getNextToken(skipSpace);
}

bool Parser::isDataType()
{
    if (getCurrentToken().isDataType())
    {
        return true;
    }

    if ((getCurrentToken().isOperatorStar() || getCurrentToken().isOperatorAnd()) && expectToken().isDataType())
    {
        return true;
    }
    return false;
}