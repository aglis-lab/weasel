#pragma once

#include "weasel/Lexer/Token.h"
#include "weasel/Basic/FileManager.h"

namespace weasel
{
    class Lexer
    {
    private:
        char *_startBuffer;
        char *_endBuffer;
        char *_currentBuffer;

        Token _currentToken;
        SourceLocation _location;

    private:
        // Get and Next Buffer
        inline char *getNextBuffer(size_t slide = 1);
        inline char checkNextBuffer() const { return *(_currentBuffer + 1); }
        inline void setCurrentBuffer(char *buffer);

        bool compareBuffer(char *startBuffer, char *endBuffer, const char *compareBuffer);
        bool isIdentifier(char c, bool num = false);
        inline bool isValidBuffer() const { return _endBuffer - _currentBuffer > 0; }

        // TODO: Handle special character fir parseCharacter or string
        // Token section
        Token getToken();
        Token createToken(TokenKind kind, char *startBuffer, char *endBuffer);
        Token getType(char *startBuffer, char *endBuffer);
        Token getKeyword(char *startBuffer, char *endBuffer);
        Token getPunctuation();
        Token getStringLiteral();
        Token getCharacterLiteral();

    public:
        Lexer(FileManager *fileManager);

        Token getNextToken(bool skipSpace = false, bool eat = false);
        Token getCurrentToken() const { return _currentToken; }
        bool expect(TokenKind kind);
    };

} // namespace weasel