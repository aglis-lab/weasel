#include "weasel/Lexer/Lexer.h"

weasel::Lexer::Lexer(FileManager *fileManager)
{
    _currentBuffer = _startBuffer = fileManager->getStartBuffer();
    _endBuffer = _startBuffer + fileManager->getSize();
}

bool weasel::Lexer::compareBuffer(char *startBuffer, char *endBuffer, const char *compareBuffer)
{
    auto length = endBuffer - startBuffer;

    if (length != ((long)strlen(compareBuffer)))
    {
        return false;
    }

    for (auto i = 0; i < length; i++)
    {
        if (startBuffer[i] != compareBuffer[i])
        {
            return false;
        }
    }

    return true;
}

bool weasel::Lexer::isIdentifier(char c, bool num)
{
    if (c == '_')
        return true;

    return num ? isalnum(c) : isalpha(c);
}

char *weasel::Lexer::getNextBuffer(size_t slide)
{
    if (this->isNewline())
    {
        _location.newLine();
        _location.incrementColumn(slide - 1);
    }
    else
    {
        _location.incrementColumn(slide);
    }

    _currentBuffer += slide; // get next buffer

    return _currentBuffer;
}

bool weasel::Lexer::expect(weasel::TokenKind kind)
{
    auto lastBuffer = _currentBuffer;
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

weasel::Token weasel::Lexer::createToken(weasel::TokenKind kind, char *startBuffer, char *endBuffer)
{
    return Token::create(kind, _location, startBuffer, endBuffer);
}

weasel::Token weasel::Lexer::getNextToken(bool skipSpace)
{
    do
    {
        _currentToken = getToken();
    } while (_currentToken.isNewline() && skipSpace);

    return _currentToken;
}

weasel::Token weasel::Lexer::getToken()
{
    if (_currentBuffer == _endBuffer)
    {
        return this->createToken(TokenKind::TokenEOF, _currentBuffer, _endBuffer);
    }

    while (isspace(*_currentBuffer))
    {
        if (*_currentBuffer == '\n')
        {
            // New Line always come at the end of code
            // getNextChar will force new character from user input
            // And that's not what we want
            // Instead make lastChar empty char will not need new character
            // And next iteration will be ignored
            getNextBuffer();
            return this->createToken(TokenKind::TokenSpaceNewline, _currentBuffer - 1, _currentBuffer);
        }

        getNextBuffer();
    }

    // Check if identifier
    if (isIdentifier(*_currentBuffer))
    {
        auto *start = _currentBuffer;
        while (isIdentifier(*getNextBuffer(), true))
            ;

        /// Check Keyword ///
        auto token = getKeyword(start, _currentBuffer);
        if (!token.isUnknown())
        {
            return token;
        }

        // Check if data type
        token = getType(start, _currentBuffer);
        if (!token.isUnknown())
        {
            return token;
        }

        TokenKind kind = TokenKind::TokenIdentifier;
        // Check NIl Literal
        if (compareBuffer(start, _currentBuffer, "nil"))
        {
            kind = TokenKind::TokenLitNil;
        }
        else if (compareBuffer(start, _currentBuffer, "true") || compareBuffer(start, _currentBuffer, "false"))
        {
            // Check Boolean Literal
            kind = TokenKind::TokenLitBool;
        }

        // Identifier
        return this->createToken(kind, start, _currentBuffer);
    }

    // Check if Number
    if (isdigit(*_currentBuffer) || (*_currentBuffer == '.' && isdigit(checkNextBuffer())))
    {
        auto start = _currentBuffer;
        auto numDot = 0;

        do
        {
            if (*_currentBuffer == '.')
            {
                numDot++;
            }

            getNextBuffer();
        } while (isdigit(*_currentBuffer) || *_currentBuffer == '.');

        if (numDot >= 2)
        {
            return this->createToken(TokenKind::TokenUnknown, start, _currentBuffer);
        }

        if (*_currentBuffer == 'd')
        {
            getNextBuffer(); // eat 'd' for double

            return this->createToken(TokenKind::TokenLitDouble, start, _currentBuffer);
        }

        if (numDot == 1 || *_currentBuffer == 'f')
        {
            if (*_currentBuffer == 'f')
            {
                getNextBuffer(); // eat 'f' if exist
            }

            return this->createToken(TokenKind::TokenLitFloat, start, _currentBuffer);
        }

        // Number Literal
        return this->createToken(TokenKind::TokenLitInteger, start, _currentBuffer);
    }

    // String Literal
    if (*_currentBuffer == '"')
    {
        return getStringLiteral();
    }

    // Character Literal
    if (*_currentBuffer == '\'')
    {
        return getCharacterLiteral();
    }

    // Save Current Buffer
    auto *lastBuffer = _currentBuffer;
    getNextBuffer();

    // TODO: You need to save some comment to make a documentation
    // Single Line Comment
    if (*lastBuffer == '/' && *_currentBuffer == '/')
    {
        while (*getNextBuffer() != '\n' && isValidBuffer())
            ;
        return getToken();
    }

    // TODO: You need to save some comment to make a documentation
    // TODO: Should Error when EOF
    // Multiple Line Comment
    if (*lastBuffer == '/' && *_currentBuffer == '*')
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

            if (*lastBuffer == '*' && *_currentBuffer == '/')
            {
                getNextBuffer(); // eat /
                return getToken();
            }

            lastBuffer = _currentBuffer;
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

    auto *start = _currentBuffer - 1;
    while (!isspace(*_currentBuffer))
    {
        getNextBuffer();
    }

    return this->createToken(TokenKind::TokenUnknown, start, _currentBuffer);
}
