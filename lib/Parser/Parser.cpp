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

// TODO: Find User Type
// weasel::StructType *weasel::Parser::findUserType(const std::string &typeName)
// {
//     for (auto item : getUserTypes())
//     {
//         if (item->getIdentifier() == typeName)
//         {
//             return item;
//         }
//     }

//     return nullptr;
// }

// FunctionHandle Parser::findFunction(const string &identifier, StructType *structType, bool isStatic)
// {
//     for (auto item : getFunctions())
//     {
//         auto checkStatic = item->getIsStatic() == isStatic;
//         auto checkStruct = item->getImplStruct() == structType;
//         auto checkIdent = item->getIdentifier() == identifier;
//         if (checkIdent && checkStruct && checkStatic)
//         {
//             return item;
//         }
//     }

//     return nullptr;
// }
