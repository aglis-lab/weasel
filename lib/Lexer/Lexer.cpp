#include "weasel/Lexer/Lexer.h"

bool Lexer::compareBuffer(char *startBuffer, char *endBuffer, const char *compareBuffer)
{
    return string_view(startBuffer, endBuffer) == compareBuffer;
}

bool Lexer::isIdentifier(char c, bool num)
{
    if (c == '_')
        return true;

    return num ? isalnum(c) : isalpha(c);
}

// This Method can't support newline '\n' at middle of slide
// ex:
// ab\nf
// if slide 4 that's mean newline is at a middle of slide
char *Lexer::getNextBuffer(size_t slide)
{
    if (isLastNewline())
    {
        _location.newLine();
        _location.incrementColumn(slide);
    }
    else
    {
        _location.incrementColumn(slide);
    }

    _currentBuffer += slide; // get next buffer

    return getCurrentBuffer();
}

Token Lexer::expect()
{
    auto lastBuffer = getCurrentBuffer();
    auto ok = true;
    auto token = getToken();
    while (token.isKind(TokenKind::TokenSpaceNewline))
    {
        token = getToken();
    }

    setCurrentBuffer(lastBuffer);
    return token;
}

bool Lexer::expect(TokenKind kind)
{
    auto lastBuffer = getCurrentBuffer();
    auto ok = true;
    auto token = getToken();
    while (token.isKind(TokenKind::TokenSpaceNewline))
    {
        token = getToken();
    }

    if (!token.isKind(kind))
    {
        ok = false;
    }

    setCurrentBuffer(lastBuffer);
    return ok;
}

Token Lexer::createToken(TokenKind kind, char *startBuffer, char *endBuffer)
{
    uint tokenStartColumn = _location.column - ((endBuffer - startBuffer) - 1);

    return Token::create(kind, startBuffer, endBuffer);
}

Token Lexer::getNextToken(bool skipSpace)
{
    do
    {
        _currentToken = getToken();
    } while (_currentToken.isNewline() && skipSpace);

    return _currentToken;
}

Token Lexer::getToken()
{
    if (getCurrentBuffer() == getEndBuffer())
    {
        return createToken(TokenKind::TokenEOF, getCurrentBuffer(), getEndBuffer());
    }

    while (isspace(*getCurrentBuffer()))
    {
        if (*getCurrentBuffer() == '\n')
        {
            // New Line always come at the end of code
            // getNextChar will force new character from user input
            // And that's not what we want
            // Instead make lastChar empty char will not need new character
            // And next iteration will be ignored
            getNextBuffer();
            return createToken(TokenKind::TokenSpaceNewline, getCurrentBuffer() - 1, getCurrentBuffer());
        }

        getNextBuffer();
    }

    // Check if identifier
    if (isIdentifier(*getCurrentBuffer()))
    {
        auto *start = getCurrentBuffer();
        while (isIdentifier(*getNextBuffer(), true))
            ;

        /// Check Keyword ///
        auto token = getKeyword(start, getCurrentBuffer());
        if (!token.isUnknown())
        {
            return token;
        }

        // Check if data type
        token = getType(start, getCurrentBuffer());
        if (!token.isUnknown())
        {
            return token;
        }

        TokenKind kind = TokenKind::TokenIdentifier;
        // Check NIl Literal
        if (compareBuffer(start, getCurrentBuffer(), "nil"))
        {
            kind = TokenKind::TokenLitNil;
        }
        else if (compareBuffer(start, getCurrentBuffer(), "true") || compareBuffer(start, getCurrentBuffer(), "false"))
        {
            // Check Boolean Literal
            kind = TokenKind::TokenLitBool;
        }

        // Identifier
        return createToken(kind, start, getCurrentBuffer());
    }

    // Check if Number
    if (isdigit(*getCurrentBuffer()) || (*getCurrentBuffer() == '.' && isdigit(checkNextBuffer())))
    {
        auto start = getCurrentBuffer();
        auto numDot = 0;

        do
        {
            if (*getCurrentBuffer() == '.')
            {
                numDot++;
            }

            getNextBuffer();
        } while (isdigit(*getCurrentBuffer()) || *getCurrentBuffer() == '.');

        if (numDot >= 2)
        {
            return createToken(TokenKind::TokenUnknown, start, getCurrentBuffer());
        }

        if (*getCurrentBuffer() == 'd')
        {
            getNextBuffer(); // eat 'd' for double

            return createToken(TokenKind::TokenLitDouble, start, getCurrentBuffer());
        }

        if (numDot == 1 || *getCurrentBuffer() == 'f')
        {
            if (*getCurrentBuffer() == 'f')
            {
                getNextBuffer(); // eat 'f' if exist
            }

            return createToken(TokenKind::TokenLitFloat, start, getCurrentBuffer());
        }

        // Number Literal
        return createToken(TokenKind::TokenLitInteger, start, getCurrentBuffer());
    }

    // String Literal
    if (*getCurrentBuffer() == '"')
    {
        return getStringLiteral();
    }

    // Character Literal
    if (*getCurrentBuffer() == '\'')
    {
        return getCharacterLiteral();
    }

    // Save Current Buffer
    auto *lastBuffer = getCurrentBuffer();
    getNextBuffer();

    // TODO: You need to save some comment to make a documentation
    // Single Line Comment
    if (*lastBuffer == '/' && *getCurrentBuffer() == '/')
    {
        while (*getNextBuffer() != '\n' && isValidBuffer())
            ;
        return getToken();
    }

    // TODO: You need to save some comment to make a documentation
    // TODO: Should Error when EOF
    // Multiple Line Comment
    if (*lastBuffer == '/' && *getCurrentBuffer() == '*')
    {
        lastBuffer = getNextBuffer();
        if (!isValidBuffer())
        {
            return getToken();
        }

        while (true)
        {
            getNextBuffer();
            if (!isValidBuffer())
            {
                return getToken();
            }

            if (*lastBuffer == '*' && *getCurrentBuffer() == '/')
            {
                getNextBuffer(); // eat /
                return getToken();
            }

            lastBuffer = getCurrentBuffer();
        }
    }

    if (ispunct(*lastBuffer))
    {
        auto token = getPunctuation();
        if (!token.isUnknown())
        {
            return token;
        }
    }

    auto *start = getCurrentBuffer() - 1;
    while (!isspace(*getCurrentBuffer()))
    {
        getNextBuffer();
    }

    return createToken(TokenKind::TokenUnknown, start, getCurrentBuffer());
}
