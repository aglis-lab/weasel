#include "weasel/Lexer/Lexer.h"

Token Lexer::getType(char *startBuffer, char *endBuffer)
{
    string_view tokenVal(startBuffer, endBuffer);
    TokenKind kind;
    if (tokenVal == "rune")
    {
        kind = TokenKind::TokenTyRune;
    }
    else if (tokenVal == "byte")
    {
        kind = TokenKind::TokenTyByte;
    }
    else if (tokenVal == "sbyte")
    {
        kind = TokenKind::TokenTySbyte;
    }
    else if (tokenVal == "short")
    {
        kind = TokenKind::TokenTyShort;
    }
    else if (tokenVal == "ushort")
    {
        kind = TokenKind::TokenTyUshort;
    }
    else if (tokenVal == "int")
    {
        kind = TokenKind::TokenTyInt;
    }
    else if (tokenVal == "uint")
    {
        kind = TokenKind::TokenTyUint;
    }
    else if (tokenVal == "long")
    {
        kind = TokenKind::TokenTyLong;
    }
    else if (tokenVal == "ulong")
    {
        kind = TokenKind::TokenTyUlong;
    }
    else if (tokenVal == "int128")
    {
        kind = TokenKind::TokenTyInt128;
    }
    else if (tokenVal == "bool")
    {
        kind = TokenKind::TokenTyBool;
    }
    else if (tokenVal == "float")
    {
        kind = TokenKind::TokenTyFloat;
    }
    else if (tokenVal == "double")
    {
        kind = TokenKind::TokenTyDouble;
    }
    else if (tokenVal == "decimal")
    {
        kind = TokenKind::TokenTyDecimal;
    }
    else if (tokenVal == "void")
    {
        kind = TokenKind::TokenTyVoid;
    }
    else if (tokenVal == "any")
    {
        kind = TokenKind::TokenTyAny;
    }
    else
    {
        return Token::empty();
    }

    return this->createToken(kind, startBuffer, endBuffer);
}
