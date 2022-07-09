#include "weasel/Lexer/Lexer.h"

weasel::Token *weasel::Lexer::getStringLiteral()
{
    auto *start = getNextBuffer(); // eat double quote (")
    while (*getNextBuffer() != '"')
        ;

    auto *endString = _currentBuffer;

    getNextBuffer(); // eat double quote (")
    return createToken(TokenKind::TokenLitString, start, endString);
}

weasel::Token *weasel::Lexer::getCharacterLiteral()
{
    auto *start = getNextBuffer(); // eat single quote (')
    if (*start == '\'')
    {
        return nullptr;
    }

    if (*getNextBuffer() != '\'')
    {
        return nullptr;
    }

    getNextBuffer(); // eat single quote (')
    return createToken(TokenKind::TokenLitChar, start, start + 1);
}
