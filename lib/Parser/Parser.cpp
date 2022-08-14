#include <iostream>
#include <unistd.h>
#include "llvm/IR/Verifier.h"
#include "weasel/Parser/Parser.h"
#include "weasel/IR/Context.h"
#include "weasel/Symbol/Symbol.h"

// parse
void weasel::Parser::parse()
{
    while (!getNextToken().isEnd())
    {
        if (getCurrentToken().isKeyFunction())
        {
            addFunction(parseFunction());
        }

        if (getCurrentToken().isKeyStruct())
        {
            parseStruct();
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
weasel::Token weasel::Parser::getNextToken(bool skipSpace)
{
    return _lexer->getNextToken(skipSpace);
}
