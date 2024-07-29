#include "weasel/Lexer/Lexer.h"

Token Lexer::getStringLiteral()
{
    auto start = getNextBuffer(); // eat double quote (")
    while (getCurrentBuffer() != getEndBuffer() && *getCurrentBuffer() != '"')
    {
        getNextBuffer();
    }

    auto endString = _currentBuffer;
    if (*getCurrentBuffer() != '"')
    {
        return createToken(TokenKind::TokenUnknown, start, endString);
    }

    getNextBuffer(); // eat double quote (")
    return createToken(TokenKind::TokenLitString, start, endString);
}

Token Lexer::getCharacterLiteral()
{
    auto *start = getNextBuffer(); // eat single quote (')
    if (*start == '\'')
    {
        return Token::empty();
    }

    if (*getNextBuffer() != '\'')
    {
        return Token::empty();
    }

    getNextBuffer(); // eat single quote (')
    return this->createToken(TokenKind::TokenLitChar, start, start + 1);
}
