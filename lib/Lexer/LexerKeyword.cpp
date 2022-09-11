#include <iostream>
#include "weasel/Lexer/Lexer.h"

weasel::Token weasel::Lexer::getKeyword(char *startBuffer, char *endBuffer)
{
    if (compareBuffer(startBuffer, endBuffer, "parallel"))
    {
        return createToken(TokenKind::TokenKeyParallel, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "kernel"))
    {
        return createToken(TokenKind::TokenKeyKernel, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "inline"))
    {
        return createToken(TokenKind::TokenKeyInline, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "fun"))
    {
        return createToken(TokenKind::TokenKeyFun, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "const"))
    {
        return createToken(TokenKind::TokenKeyConst, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "extern"))
    {
        return createToken(TokenKind::TokenKeyExtern, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "return"))
    {
        return createToken(TokenKind::TokenKeyReturn, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "let"))
    {
        return createToken(TokenKind::TokenKeyLet, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "final"))
    {
        return createToken(TokenKind::TokenKeyFinal, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "if"))
    {
        return createToken(TokenKind::TokenKeyIf, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "else"))
    {
        return createToken(TokenKind::TokenKeyElse, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "for"))
    {
        return createToken(TokenKind::TokenKeyFor, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "break"))
    {
        return createToken(TokenKind::TokenKeyBreak, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "continue"))
    {
        return createToken(TokenKind::TokenKeyContinue, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "struct"))
    {
        return createToken(TokenKind::TokenKeyStruct, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "defer"))
    {
        return createToken(TokenKind::TokenKeyDefer, startBuffer, endBuffer);
    }
    else if (compareBuffer(startBuffer, endBuffer, "as"))
    {
        return createToken(TokenKind::TokenOperatorCasting, startBuffer, endBuffer);
    }

    return Token::empty();
}
