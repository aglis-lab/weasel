#pragma once

#include <string>

#include "weasel/Lexer/Token.h"

namespace weasel
{
    enum class ErrorKind
    {
        LexicalError,
        SyntaxError,
        SemanticError
    };

    // TODO: Add Range Location
    // TODO: Optimizing Error Class
    class Error
    {
    private:
        uint _errorCode;
        ErrorKind _errorKind;
        std::string _message;
        Token _token = Token::create();

    public:
        Error(uint errorCode, ErrorKind errorKind, std::string message) : _errorCode(errorCode), _message(message), _errorKind(errorKind) {}

        std::string getMessage() const { return _message; }
        uint getErrorCode() const { return _errorCode; }
        ErrorKind GetErrorKind() const { return _errorKind; }
        Token getToken() const { return _token; }

        Error withToken(Token token)
        {
            _token = token;
            return *this;
        }
    };

    class Errors
    {
    public:
        Error expectedOpenParen = Error(0, ErrorKind::SyntaxError, "expected ( for method call");
        Error expectedArgument = Error(1, ErrorKind::SyntaxError, "expected argument expression");
        Error expectedCloseParen = Error(2, ErrorKind::SyntaxError, "expected ) or , in argument list");
        Error funCallExpectedOpenParen = Error(3, ErrorKind::SyntaxError, "expected ( for function call");
        Error expectedOpenCurly = Error(11, ErrorKind::SyntaxError, "expected '{'");
        Error expectedCloseSquare = Error(5, ErrorKind::SyntaxError, "expected ']'");
        Error expectedDot = Error(6, ErrorKind::SyntaxError, "expected '.'");
        Error expectedIdentifier = Error(7, ErrorKind::SyntaxError, "expected identifier");
        Error expectedColon = Error(8, ErrorKind::SyntaxError, "expected colon");
        Error expectedComma = Error(9, ErrorKind::SyntaxError, "expected comma");
        Error expectedExpression = Error(10, ErrorKind::SyntaxError, "expected expression");

    private:
        Errors() {}

    public:
        static Errors &getInstance()
        {
            static Errors instance;

            return instance;
        }

        ~Errors() {}
        Errors(Errors const &) = delete;
        void operator=(Errors const &) = delete;
    };
} // namespace weasel
