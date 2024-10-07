#pragma once

#include <weasel/Type/Type.h>
#include <weasel/Package/Package.h>
#include <weasel/Parser/Parser.h>

namespace weasel
{
    class SemanticState
    {
    private:
        vector<uint32_t> _declarationState;
        vector<DeclarationStatement *> _declarations;

        Function *_currentFunction;

        bool _declaration = false;

    public:
        SemanticState() = default;

        void setCurrentFunction(Function *val) { _currentFunction = val; }
        Function *getCurrentFunction() { return _currentFunction; }

        void addDeclaration(DeclarationStatement *expr)
        {
            _declarations.push_back(expr);
        }
        vector<DeclarationStatement *> &getDeclarations() { return _declarations; }

        void saveState()
        {
            _declarationState.push_back(_declarations.size());
        }

        void restoreState()
        {
            _declarations.resize(_declarationState.back());
            _declarationState.pop_back();
        }

        bool getDeclaration() const { return _declaration; }
        void setDeclaration(bool val) { _declaration = val; }
    };

    class AnalysisSemantic
    {
    private:
        PackageHandle _package;
        AnalysisEvaluate *_evaluate;
        SemanticState _state;

        // TODO: Simply Use Polymorphism
        vector<Expression *> _errors;
        vector<StructType *> _typeErrors;

    public:
        explicit AnalysisSemantic(PackageHandle package) : _package(package) {}

        void semanticCheck();
        void accept(Function *fun);
        void accept(CompoundStatement *expr);
        void accept(CallExpression *expr);
        void accept(ConditionStatement *expr);
        void accept(DeclarationStatement *expr);
        void accept(VariableExpression *expr);
        void accept(AssignmentExpression *expr);
        void accept(ComparisonExpression *expr);
        void accept(ReturnExpression *expr);
        void accept(BreakExpression *expr);
        void accept(ContinueExpression *expr);
        void accept(LoopingStatement *expr);
        void accept(ArithmeticExpression *expr);
        void accept(UnaryExpression *expr);
        void accept(StructExpression *expr);
        void accept(FieldExpression *expr);
        void accept(TypeCastExpression *expr);
        void accept(ArgumentExpression *expr);
        void accept(NilLiteralExpression *expr);
        void accept(IndexExpression *expr);
        void accept(ArrayExpression *expr);
        void accept(MethodCallExpression *expr);

        void accept(FunctionType *expr);
        void accept(StructType *expr);
        void accept(ArrayType *expr);
        void accept(Type *expr);

        EvaluationValue evaluate(Expression *);
        SemanticState &getState() { return _state; }

        TypeHandle unknownType(TypeHandle expr);
        PackageHandle getPackage() { return _package; }

        DeclarationStatement *findDeclaration(string_view variableName)
        {
            for (int i = _state.getDeclarations().size() - 1; i >= 0; i--)
            {
                if (_state.getDeclarations()[i]->getIdentifier() == variableName)
                {
                    return _state.getDeclarations()[i];
                }
            }

            return nullptr;
        }

        // TODO: Change from vector of error into accept(AnalysisError)
        void onError(Expression *expr)
        {
            getErrors().push_back(expr);
        }

        void onStructError(StructType *expr)
        {
            getTypeErrors().push_back(expr);
        }

        vector<Expression *> &getErrors() { return _errors; }
        vector<StructType *> &getTypeErrors() { return _typeErrors; }
    };
} // namespace weasel
