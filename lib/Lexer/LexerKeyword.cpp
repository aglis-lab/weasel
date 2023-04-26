#include <iostream>
#include "weasel/Lexer/Lexer.h"

weasel::Token weasel::Lexer::getKeyword(char *startBuffer, char *endBuffer)
{
    if (compareBuffer(startBuffer, endBuffer, "impl"))
    {
        return Token::create(TokenKind::TokenKeyImpl, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "this"))
    {
        return Token::create(TokenKind::TokenKeyThis, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "parallel"))
    {
        return Token::create(TokenKind::TokenKeyParallel, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "kernel"))
    {
        return Token::create(TokenKind::TokenKeyKernel, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "inline"))
    {
        return Token::create(TokenKind::TokenKeyInline, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "fun"))
    {
        return Token::create(TokenKind::TokenKeyFun, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "const"))
    {
        return Token::create(TokenKind::TokenKeyConst, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "extern"))
    {
        return Token::create(TokenKind::TokenKeyExtern, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "return"))
    {
        return Token::create(TokenKind::TokenKeyReturn, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "let"))
    {
        return Token::create(TokenKind::TokenKeyLet, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "final"))
    {
        return Token::create(TokenKind::TokenKeyFinal, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "if"))
    {
        return Token::create(TokenKind::TokenKeyIf, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "else"))
    {
        return Token::create(TokenKind::TokenKeyElse, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "for"))
    {
        return Token::create(TokenKind::TokenKeyFor, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "break"))
    {
        return Token::create(TokenKind::TokenKeyBreak, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "continue"))
    {
        return Token::create(TokenKind::TokenKeyContinue, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "struct"))
    {
        return Token::create(TokenKind::TokenKeyStruct, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "defer"))
    {
        return Token::create(TokenKind::TokenKeyDefer, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "as"))
    {
        return Token::create(TokenKind::TokenOperatorCasting, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "readonly"))
    {
        return Token::create(TokenKind::TokenKeyReadOnly, _location, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "writeonly"))
    {
        return Token::create(TokenKind::TokenKeyWriteOnly, _location, startBuffer, endBuffer);
    }

    return Token::empty();
}
