#include "weasel/AST/AST.h"
#include <vector>

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
