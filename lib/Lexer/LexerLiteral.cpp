#include "weasel/Lexer/Lexer.h"

weasel::Token weasel::Lexer::getStringLiteral()
{
    auto start = getNextBuffer(); // eat double quote (")
    while (getCurrentBuffer() != getEndBuffer() && *getNextBuffer() != '"')
        ;

    auto endString = _currentBuffer;
    if (*getCurrentBuffer() != '"')
    {
        return createToken(TokenKind::TokenUnknown, start, endString);
    }

    getNextBuffer(); // eat double quote (")
    return createToken(TokenKind::TokenLitString, start, endString);
}

weasel::Token weasel::Lexer::getCharacterLiteral()
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
