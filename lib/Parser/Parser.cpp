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

    while (!getNextToken(true)->isKind(TokenKind::TokenEOF))
    {

        // Parallel Function
        // TODO: Need add parallel function
        if (getCurrentToken()->isKind(TokenKind::TokenKeyParallel))
        {
            std::cout << "Function Parallel no supported yet!\n";
            exit(1);
        }

        // Extern Function
        if (getCurrentToken()->isKind(TokenKind::TokenKeyExtern))
        {
            getNextToken(); // eat 'extern'
            auto fun = parseFunction();
            if (fun != nullptr)
            {
                funs.push_back(fun);
            }
            continue;
        }

        // Function
        if (getCurrentToken()->isKind(TokenKind::TokenKeyFun))
        {
            auto fun = parseFunction();
            if (fun != nullptr)
            {
                funs.push_back(fun);
            }
            continue;
        }

        // TODO: Doing Global Variable
        // For latter implementation
        auto token = getCurrentToken();
        std::cout << "Parser -> " << token->getLocation().row << "/" << token->getLocation().col << "<>" << token->getTokenKindToInt() << ":" << token->getValue() << "\n";
    }

    return funs;
}

// get Next Token Until
weasel::Token *weasel::Parser::getNextTokenUntil(weasel::TokenKind kind)
{
    if (getCurrentToken()->isKind(kind))
    {
        return getCurrentToken();
    }

    while (auto token = getNextToken())
    {
        if (token->isKind(kind))
        {
            return token;
        }

        if (token->isKind(TokenKind::TokenEOF))
        {
            break;
        }
    }

    return nullptr;
}

// Get Next Token
weasel::Token *weasel::Parser::getNextToken(bool skipSpace)
{
    return _lexer->getNextToken(skipSpace);
}
