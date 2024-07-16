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

        // switch (getCurrentToken().getTokenKind())
        // {
        // case TokenKind::TokenKeyStruct:
        //     addUserType(parseStruct());
        //     break;
        // case TokenKind::TokenKeyFun:
        // {
        //     auto fun = parseFunction();
        //     addFunction(fun);

        //     if (fun->isError())
        //     {
        //         return;
        //     }

        //     break;
        // }
        // case TokenKind::TokenKeyExtern:
        // {
        //     auto fun = parseExternFunction();
        //     addFunction(fun);

        //     if (fun->isError())
        //     {
        //         return;
        //     }

        //     break;
        // }
        // case TokenKind::TokenKeyImpl:
        //     parseImplFunctions();
        //     break;
        // case TokenKind::TokenKeyLet:
        // case TokenKind::TokenKeyConst:
        //     addGlobalVariable(parseGlobalVariable());
        //     break;
        // default:
        //     LOG(ERROR) << "Unexpected token : " << getCurrentToken().getTokenKindToInt() << " - " << getCurrentToken().getValue();
        //     break;
        // }
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
