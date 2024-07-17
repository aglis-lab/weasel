#include "weasel/Type/Type.h"
#include "weasel/Parser/Parser.h"

namespace weasel
{
    class AnalysisSemantic
    {
    private:
        Module *_module;

        vector<ExpressionHandle> _errors;
        vector<StructTypeHandle> _typeErrors;

    public:
        explicit AnalysisSemantic(Module *module) : _module(module) {}

        void semanticCheck();
        void statementCheck(ExpressionHandle expr);
        void compoundStatementCheck(CompoundStatementHandle expr);
        void callExpressionCheck(CallExpressionHandle expr);
        void userTypeCheck(StructTypeHandle expr);

        void onError(ExpressionHandle expr)
        {
            getErrors().push_back(expr);
        }
        void onStructError(StructTypeHandle expr)
        {
            getTypeErrors().push_back(expr);
        }

        Module *getModule() const { return _module; }

        vector<ExpressionHandle> &getErrors() { return _errors; }
        vector<StructTypeHandle> &getTypeErrors() { return _typeErrors; }
    };
} // namespace weasel
