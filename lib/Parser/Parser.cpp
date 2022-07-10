#include <iostream>
#include <unistd.h>
#include "llvm/IR/Verifier.h"
#include "weasel/Parser/Parser.h"
#include "weasel/IR/Context.h"
#include "weasel/Symbol/Symbol.h"

// parse
std::vector<weasel::Function *> weasel::Parser::parse()
{
    std::vector<weasel::Function *> funs;

    while (!getNextToken(true).isKind(TokenKind::TokenEOF))
    {
        // Function
        if (!getCurrentToken().isKind(TokenKind::TokenKeyFun))
        {
            // TODO: Doing Global Variable
            // For latter implementation
            auto token = getCurrentToken();
            std::cout << "Parser -> " << token.getLocation().row << "/" << token.getLocation().col << " <> " << token.getTokenKindToInt() << " : " << token.getValue() << "\n";

            continue;
        }

        auto fun = parseFunction();
        funs.push_back(fun);
    }

    return funs;
}

// get Next Token Until
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

        if (token.isKind(TokenKind::TokenEOF) || token.isUndefined())
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
