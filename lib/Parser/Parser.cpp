#include "weasel/Parser/Parser.h"

// parse
void Parser::parse()
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
            getModule()->addUserType(parseStruct());
        }
        else if (getCurrentToken().isKeyFunction() || getCurrentToken().isKeyExtern())
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
        else if (getCurrentToken().isKeyDeclaration())
        {
            getModule()->addGlobalVariable(parseGlobalVariable());
        }
        else if (getCurrentToken().isKeyImpl())
        {
            parseImplFunctions();
        }
    }
}

// Get Next Token Until
Token Parser::getNextTokenUntil(TokenKind kind)
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
