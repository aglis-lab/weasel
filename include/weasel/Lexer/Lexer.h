#pragma once

#include <list>

#include <weasel/Lexer/Token.h>
#include <weasel/Basic/FileManager.h>

using namespace weasel;

namespace weasel
{
    class Lexer
    {
    public:
        explicit Lexer(FileManager fileManager) : _currentBuffer(fileManager.getStartBuffer()), _fileManager(fileManager) {}

        Token getNextToken(bool skipSpace = false);
        Token getCurrentToken() const { return _currentToken; }
        bool expect(TokenKind kind);
        Token expect();

    private: // Private variable for creating currrent token and buffer
        char *_currentBuffer;
        FileManager _fileManager;

        Token _currentToken = Token::create();
        // check last token
        // if last token is newline and current token is newline
        // we just ignore the current token and continue into next token
        Token _lastToken = Token::create();
        SourceLocation _location;

    private: // Private Function
        // Get and Next Buffer
        char *getStartBuffer() const { return _fileManager.getStartBuffer(); }
        char *getCurrentBuffer() const { return _currentBuffer; }
        char *getEndBuffer() const { return _fileManager.getEndBuffer(); }
        char *getNextBuffer(size_t slide = 1);
        char checkNextBuffer() const { return *(_currentBuffer + 1); }
        void setCurrentBuffer(const char *buffer) { _currentBuffer -= _currentBuffer - buffer; }

        bool compareBuffer(char *startBuffer, char *endBuffer, const char *compareBuffer);
        bool isIdentifier(char c, bool num = false);
        bool isValidBuffer() const { return getEndBuffer() - getCurrentBuffer() > 0; }

        bool isNewline() const { return *getCurrentBuffer() == '\n'; }
        bool isLastNewline() const { return *(getCurrentBuffer() - 1) == '\n'; }

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
