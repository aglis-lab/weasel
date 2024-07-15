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

        // TODO: Not support parallel yet...
        // if (getCurrentToken().isKeyParallel())
        // {
        //     auto type = findUserType(getCurrentToken().getValue());
        //     assert(type && "parallel struct type should be defined internally!");

        //     getNextToken(true);
        //     auto fun = parseFunction(type);
        //     if (fun)
        //     {
        //         fun->setParallel(true);

        //         addFunction(fun);
        //     }
        // }

        switch (getCurrentToken().getTokenKind())
        {
        case TokenKind::TokenKeyStruct:
            addUserType(parseStruct());
            break;
        case TokenKind::TokenKeyFun:
        {
            auto fun = parseFunction();
            addFunction(fun);

            if (fun->isError())
            {
                return;
            }

            break;
        }
        case TokenKind::TokenKeyExtern:
        {
            auto fun = parseExternFunction();
            addFunction(fun);

            if (fun->isError())
            {
                return;
            }

            break;
        }
        case TokenKind::TokenKeyImpl:
            parseImplFunctions();
            break;
        case TokenKind::TokenKeyLet:
        case TokenKind::TokenKeyConst:
            addGlobalVariable(parseGlobalVariable());
            break;
        default:
            LOG(ERROR) << "Unexpected token : " << getCurrentToken().getTokenKindToInt() << " - " << getCurrentToken().getValue();
            break;
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

weasel::Function *weasel::Parser::findFunction(const std::string &identifier, StructType *structType, bool isStatic)
{
    for (auto item : getFunctions())
    {
        auto checkStatic = item->getIsStatic() == isStatic;
        auto checkStruct = item->getImplStruct() == structType;
        auto checkIdent = item->getIdentifier() == identifier;
        if (checkIdent && checkStruct && checkStatic)
        {
            return item;
        }
    }

    return nullptr;
}
