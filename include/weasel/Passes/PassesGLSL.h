#pragma once

#include <vector>

#include <weasel/AST/AST.h>

namespace weasel
{
    class PassesGLSL
    {
    private:
        std::vector<Function *> _functions;

    public:
        PassesGLSL(std::vector<Function *> functions) : _functions(functions) {}

        bool run();
    };
} // namespace weasel
