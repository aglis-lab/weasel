#include "weasel/Type/Type.h"
#include "weasel/Parser/Parser.h"

namespace weasel
{
    class AnalysisSemantic
    {
    private:
        Module *_module;

    public:
        AnalysisSemantic(Module *module) : _module(module) {}
    };
} // namespace weasel
