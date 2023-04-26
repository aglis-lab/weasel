#pragma once

#include "weasel/Lexer/Token.h"
#include "weasel/AST/ASTStatement.h"

// Function
namespace weasel
{
    // Function
    class Function : public GlobalObject
    {
    private:
        CompoundStatement *_body;
        std::vector<ArgumentType *> _arguments;

        bool _isDefine = false;
        bool _isInline = false;
        bool _isParallel = false;

    public:
        Function(std::string identifier, Type *type, std::vector<ArgumentType *> arguments) : GlobalObject(Token::create(), identifier, type), _arguments(arguments) {}

        CompoundStatement *getBody() const { return _body; }
        void setBody(CompoundStatement *body) { _body = body; }

        void setIsDefine(bool val) { _isDefine = val; }
        bool isDefine() const { return _isDefine; }

        void setIsInline(bool val) { _isInline = val; }
        bool isInline() const { return _isInline; }

        void setParallel(bool val) { _isParallel = val; }
        bool getParallel() const { return _isParallel; }

        void setArguments(std::vector<ArgumentType *> arguments) { _arguments = arguments; }
        std::vector<ArgumentType *> getArguments() const { return _arguments; }

    public:
        llvm::Value *codegen(Context *c) override;
        void debug(int shift) override;

        ~Function()
        {
            _arguments.clear();

            delete _body;
        }
    };

} // namespace weasel
