#include <iostream>

#include "weasel/Lexer/Lexer.h"

weasel::Token weasel::Lexer::getKeyword(char *startBuffer, char *endBuffer)
{
    uint keywordLength = endBuffer - startBuffer;
    uint column = _location.column - (keywordLength - 1);
    SourceLocation newLocation(startBuffer - getStartBuffer(), _location.line, column);

    string_view keyword(startBuffer, keywordLength);
    if (keyword == "impl")
    {
        return Token::create(TokenKind::TokenKeyImpl, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "this")
    {
        return Token::create(TokenKind::TokenKeyThis, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "parallel")
    {
        return Token::create(TokenKind::TokenKeyParallel, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "kernel")
    {
        return Token::create(TokenKind::TokenKeyKernel, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "inline")
    {
        return Token::create(TokenKind::TokenKeyInline, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "fun")
    {
        return Token::create(TokenKind::TokenKeyFun, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "const")
    {
        return Token::create(TokenKind::TokenKeyConst, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "extern")
    {
        return Token::create(TokenKind::TokenKeyExtern, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "return")
    {
        return Token::create(TokenKind::TokenKeyReturn, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "let")
    {
        return Token::create(TokenKind::TokenKeyLet, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "final")
    {
        return Token::create(TokenKind::TokenKeyFinal, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "if")
    {
        return Token::create(TokenKind::TokenKeyIf, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "else")
    {
        return Token::create(TokenKind::TokenKeyElse, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "for")
    {
        return Token::create(TokenKind::TokenKeyFor, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "break")
    {
        return Token::create(TokenKind::TokenKeyBreak, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "continue")
    {
        return Token::create(TokenKind::TokenKeyContinue, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "struct")
    {
        return Token::create(TokenKind::TokenKeyStruct, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "defer")
    {
        return Token::create(TokenKind::TokenKeyDefer, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "as")
    {
        return Token::create(TokenKind::TokenOperatorCasting, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "readonly")
    {
        return Token::create(TokenKind::TokenKeyReadOnly, newLocation, startBuffer, endBuffer);
    }
    else if (keyword == "writeonly")
    {
        return Token::create(TokenKind::TokenKeyWriteOnly, newLocation, startBuffer, endBuffer);
    }

    return Token::empty();
}
