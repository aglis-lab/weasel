#pragma once

#include <list>

#include <weasel/Lexer/Token.h>
#include <weasel/Basic/FileManager.h>

namespace weasel
{
    class Lexer
    {
    public:
        Lexer(FileManager *fileManager);

        Token getNextToken(bool skipSpace = false);
        Token getCurrentToken() const { return _currentToken; }
        bool expect(TokenKind kind);

    private: // Private variable for creating currrent token and buffer
        char *_startBuffer;
        char *_endBuffer;
        char *_currentBuffer;

        Token _currentToken = Token::create();
        // check last token
        // if last token is newline and current token is newline
        // we just ignore the current token and continue into next token
        Token _lastToken = Token::create();
        SourceLocation _location;

    private: // Private Function
        // Get and Next Buffer
        char *getNextBuffer(size_t slide = 1);
        inline char checkNextBuffer() const { return *(_currentBuffer + 1); }
        inline void setCurrentBuffer(char *buffer) { _currentBuffer -= _currentBuffer - buffer; }

        bool compareBuffer(char *startBuffer, char *endBuffer, const char *compareBuffer);
        bool isIdentifier(char c, bool num = false);
        inline bool isValidBuffer() const { return _endBuffer - _currentBuffer > 0; }

        // TODO: Handle special character for parseCharacter or string
        // Token section
        Token getToken();
        Token getType(char *startBuffer, char *endBuffer);
        Token getKeyword(char *startBuffer, char *endBuffer);
        Token getPunctuation();
        Token getStringLiteral();
        Token getCharacterLiteral();

        Token createToken(TokenKind kind, char *startBuffer, char *endBuffer);
    };
} // namespace weasel
