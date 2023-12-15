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
        uint _errorCode;
        std::string _message;
        RangeLocation _rangeLocation;
        Expression *_expression;
        std::string _additionalInfo;

    public:
        Error(uint errorCode, std::string &message) : _errorCode(errorCode), _message(message) {}

        void setExpression(Expression *expression)
        {
            _expression = expression;
        }

        std::string getMessage() const { return _message; }
    };
} // namespace weasel
