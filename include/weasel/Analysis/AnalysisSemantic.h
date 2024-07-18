#include "weasel/Type/Type.h"
#include "weasel/Parser/Parser.h"

namespace weasel
{
    using ConditionStatementHandle = shared_ptr<ConditionStatement>;
    using DeclarationStatementHandle = shared_ptr<DeclarationStatement>;
    using VariableExpressionHandle = shared_ptr<VariableExpression>;
    using AssignmentExpressionHandle = shared_ptr<AssignmentExpression>;

    class AnalysisSemantic
    {
    private:
        Module *_module;

        vector<ExpressionHandle> _errors;
        vector<StructTypeHandle> _typeErrors;

        vector<DeclarationStatementHandle> _declarations;

    public:
        explicit AnalysisSemantic(Module *module) : _module(module) {}

        void semanticCheck();
        void expressionCheck(ExpressionHandle expr);
        void compoundStatementCheck(CompoundStatementHandle expr);
        void callExpressionCheck(CallExpressionHandle expr);
        void userTypeCheck(StructTypeHandle expr);
        void conditionStatementChech(ConditionStatementHandle expr);
        void declarationStatementCheck(DeclarationStatementHandle expr);
        void variableExpressionCheck(VariableExpressionHandle expr);
        void assignmentExpressionCheck(AssignmentExpressionHandle expr);

        void onError(ExpressionHandle expr)
        {
            getErrors().push_back(expr);
        }
        void onStructError(StructTypeHandle expr)
        {
            getTypeErrors().push_back(expr);
        }

        Module *getModule() const { return _module; }
        vector<DeclarationStatementHandle> &getDeclarations() { return _declarations; }
        DeclarationStatementHandle findDeclaration(string_view variableName)
        {
            for (int i = getDeclarations().size() - 1; i >= 0; i--)
            {
                if (getDeclarations()[i]->getIdentifier() == variableName)
                {
                    return getDeclarations()[i];
                }

                return nullptr;
            }
        }

        vector<ExpressionHandle> &getErrors() { return _errors; }
        vector<StructTypeHandle> &getTypeErrors() { return _typeErrors; }
    };
} // namespace weasel
