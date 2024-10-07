#include <iostream>

#include "weasel/Lexer/Lexer.h"

Token Lexer::getKeyword(char *startBuffer, char *endBuffer)
{
    uint keywordLength = endBuffer - startBuffer;
    string_view keyword(startBuffer, keywordLength);
    if (keyword == "impl")
    {
        return Token::create(TokenKind::TokenKeyImpl, startBuffer, endBuffer);
    }
    else if (keyword == "this")
    {
        return Token::create(TokenKind::TokenKeyThis, startBuffer, endBuffer);
    }
    else if (keyword == "parallel")
    {
        return Token::create(TokenKind::TokenKeyParallel, startBuffer, endBuffer);
    }
    else if (keyword == "kernel")
    {
        return Token::create(TokenKind::TokenKeyKernel, startBuffer, endBuffer);
    }
    else if (keyword == "inline")
    {
        return Token::create(TokenKind::TokenKeyInline, startBuffer, endBuffer);
    }
    else if (keyword == "fun")
    {
        return Token::create(TokenKind::TokenKeyFun, startBuffer, endBuffer);
    }
    else if (keyword == "const")
    {
        return Token::create(TokenKind::TokenKeyConst, startBuffer, endBuffer);
    }
    else if (keyword == "extern")
    {
        return Token::create(TokenKind::TokenKeyExtern, startBuffer, endBuffer);
    }
    else if (keyword == "return")
    {
        return Token::create(TokenKind::TokenKeyReturn, startBuffer, endBuffer);
    }
    else if (keyword == "let")
    {
        return Token::create(TokenKind::TokenKeyLet, startBuffer, endBuffer);
    }
    else if (keyword == "final")
    {
        return Token::create(TokenKind::TokenKeyFinal, startBuffer, endBuffer);
    }
    else if (keyword == "if")
    {
        return Token::create(TokenKind::TokenKeyIf, startBuffer, endBuffer);
    }
    else if (keyword == "else")
    {
        return Token::create(TokenKind::TokenKeyElse, startBuffer, endBuffer);
    }
    else if (keyword == "for")
    {
        return Token::create(TokenKind::TokenKeyFor, startBuffer, endBuffer);
    }
    else if (keyword == "break")
    {
        return Token::create(TokenKind::TokenKeyBreak, startBuffer, endBuffer);
    }
    else if (keyword == "continue")
    {
        return Token::create(TokenKind::TokenKeyContinue, startBuffer, endBuffer);
    }
    else if (keyword == "struct")
    {
        return Token::create(TokenKind::TokenKeyStruct, startBuffer, endBuffer);
    }
    else if (keyword == "defer")
    {
        return Token::create(TokenKind::TokenKeyDefer, startBuffer, endBuffer);
    }
    else if (keyword == "as")
    {
        return Token::create(TokenKind::TokenOperatorCasting, startBuffer, endBuffer);
    }
    else if (keyword == "readonly")
    {
        return Token::create(TokenKind::TokenKeyReadOnly, startBuffer, endBuffer);
    }
    else if (keyword == "writeonly")
    {
        return Token::create(TokenKind::TokenKeyWriteOnly, startBuffer, endBuffer);
    }

    return Token::empty();
}
