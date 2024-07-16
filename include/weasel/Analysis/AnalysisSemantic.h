#include "weasel/Type/Type.h"
#include "weasel/Parser/Parser.h"

namespace weasel
{
    class AnalysisSemantic
    {
    private:
        Module *_module;

        vector<ExpressionHandle> _errors;

    public:
        explicit AnalysisSemantic(Module *module) : _module(module) {}

        void semanticCheck();
        void statementCheck(ExpressionHandle expr);
        void compoundStatementCheck(CompoundStatementHandle expr);
        void callExpressionCheck(CallExpressionHandle expr);

        void onError(ExpressionHandle expr);

        Module *getModule() const { return _module; }

        vector<ExpressionHandle> &getErrors() { return _errors; }
    };
} // namespace weasel
