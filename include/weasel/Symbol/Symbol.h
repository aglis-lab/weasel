#pragma once

#include <utility>
#include <vector>
#include <string>

#include <llvm/IR/Value.h>

#include "weasel/Lexer/Lexer.h"
#include "weasel/Type/Type.h"
#include "weasel/Basic/Error.h"

namespace weasel
{
    // class ErrorTable
    // {
    // private:
    //     ErrorTable() = default;

    //     static std::vector<Error> _errors;

    // public:
    //     static void showErrors();
    //     static std::vector<Error> getErrors() { return _errors; }
    //     static std::nullptr_t addError(Token token, std::string msg);
    // };
} // namespace weasel
