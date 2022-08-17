#include "weasel/Type/Type.h"
#include "weasel/Parser/Parser.h"

namespace weasel
{
    class AnalysisSemantic
    {
    private:
        Parser *_parser;

    public:
        AnalysisSemantic(Parser *parser) : _parser(parser) {}
    };
} // namespace weasel
