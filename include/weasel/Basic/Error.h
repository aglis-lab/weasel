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

    class Error
    {
    private:
        uint _errorCode;
        ErrorKind _errorKind;
        string _message;
        Token _token = Token::create();

    public:
        Error(uint errorCode, ErrorKind errorKind, const std::string &message) : _errorCode(errorCode), _errorKind(errorKind), _message(message) {}

        string getMessage() const { return _message; }
        uint getErrorCode() const { return _errorCode; }
        ErrorKind GetErrorKind() const { return _errorKind; }
        Token getToken() const { return _token; }

        Error withToken(const Token &token)
        {
            _token = token;
            return *this;
        }
    };

    class Errors
    {
    public:
        // SYNTAX ERROR
        Error unimplementedSyntax = Error(-1, ErrorKind::SyntaxError, "unimplemented syntax");
        Error expectedOpenParen = Error(0, ErrorKind::SyntaxError, "expected '(' for method call");
        Error expectedArgument = Error(1, ErrorKind::SyntaxError, "expected argument expression");
        Error expectedCloseParen = Error(2, ErrorKind::SyntaxError, "expected ')' or , in argument list");
        Error expectedOpenCurly = Error(4, ErrorKind::SyntaxError, "expected '{'");
        Error expectedCloseSquare = Error(5, ErrorKind::SyntaxError, "expected ']'");
        Error expectedDot = Error(6, ErrorKind::SyntaxError, "expected '.'");
        Error expectedIdentifier = Error(7, ErrorKind::SyntaxError, "expected identifier");
        Error expectedColon = Error(8, ErrorKind::SyntaxError, "expected colon");
        Error expectedComma = Error(9, ErrorKind::SyntaxError, "expected comma");
        Error expectedExpression = Error(10, ErrorKind::SyntaxError, "expected expression");
        Error returnTypeNotValid = Error(11, ErrorKind::SyntaxError, "return type not valid");
        Error invalidVararg = Error(12, ErrorKind::SyntaxError, "variadic number argument should be final argument");
        Error expectedDataType = Error(13, ErrorKind::SyntaxError, "expected data type");
        Error expectedNewLine = Error(14, ErrorKind::SyntaxError, "expected newline");
        Error expectedDefaultValue = Error(15, ErrorKind::SyntaxError, "data type expected for default value declaration");
        Error expectedEqualSign = Error(16, ErrorKind::SyntaxError, "expected equal sign");
        Error expectedRHSValue = Error(17, ErrorKind::SyntaxError, "expected RHS value expression");

        // SEMANTIC ERROR
        Error functionNotDefined = Error(100, ErrorKind::SemanticError, "function not declared");
        Error datatypeDifferent = Error(101, ErrorKind::SemanticError, "data type different");
        Error variableNotDefined = Error(102, ErrorKind::SemanticError, "variable isn't defined");
        Error lhsNotAssignable = Error(103, ErrorKind::SemanticError, "left expression isn't assignable");

    private:
        Errors() = default;

    public:
        static Errors &getInstance()
        {
            static Errors instance;

            return instance;
        }

        ~Errors() = default;
        Errors(Errors const &) = delete;
        void operator=(Errors const &) = delete;
    };
} // namespace weasel
