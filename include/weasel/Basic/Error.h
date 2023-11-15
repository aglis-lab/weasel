#pragma once

#include <string>

#include "weasel/Lexer/Token.h"

namespace weasel
{
    enum class ErrorKind
    {
        LexixalError,
        SyntaxError,
        SemanticError
    };

    class Error
    {
    private:
        Token _token;
        std::string _msg;

    public:
        Error(Token token, std::string &msg) : _token(token), _msg(msg) {}
        Error(Token token, const char *msg) : _token(token), _msg(std::string(msg)) {}

        Token getToken() const { return _token; }
        std::string getMessage() const { return _msg; }
    };
} // namespace weasel
