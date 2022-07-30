#include <utility>
#include <vector>
#include <string>
#include "llvm/IR/Value.h"
#include "weasel/Lexer/Lexer.h"
#include "weasel/Type/Type.h"

// Error
namespace weasel
{
    // Error
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

    // Error Table
    class ErrorTable
    {
    private:
        ErrorTable() = default;

        static std::vector<Error> _errors;

    public:
        static void showErrors();
        static std::vector<Error> getErrors() { return _errors; }
        static std::nullptr_t addError(Token token, std::string msg);
    };

} // namespace weasel
