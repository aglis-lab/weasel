#pragma once

#include "weasel/Type/Type.h"
#include "weasel/Parser/Parser.h"

namespace weasel
{
    class AnalysisSemantic
    {
    private:
        Module *_module;

        // TODO: Simply Use Polymorphism
        vector<Expression *> _errors;
        vector<StructType *> _typeErrors;

        // TODO: Make better State Class
        vector<uint32_t> _declarationState;
        vector<DeclarationStatement *> _declarations;

        Function *_currentFunction;

    public:
        explicit AnalysisSemantic(Module *module) : _module(module) {}

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
        void accept(Type *expr);

        TypeHandle unknownType(TypeHandle expr);

        void onError(Expression *expr)
        {
            getErrors().push_back(expr);
        }

        void onStructError(StructType *expr)
        {
            getTypeErrors().push_back(expr);
        }

        Module *getModule() const { return _module; }

        void setCurrentFunction(Function *fun) { _currentFunction = fun; }
        Function *getCurrentFunction() { return _currentFunction; }

        void saveState()
        {
            _declarationState.push_back(_declarations.size());
        }

        void restoreState()
        {
            _declarations.resize(_declarationState.back());
            _declarationState.pop_back();
        }

        vector<DeclarationStatement *> &getDeclarations()
        {
            return _declarations;
        }

        void addDeclaration(DeclarationStatement *expr)
        {
            _declarations.push_back(expr);
        }

        DeclarationStatement *findDeclaration(string_view variableName)
        {
            for (int i = getDeclarations().size() - 1; i >= 0; i--)
            {
                if (getDeclarations()[i]->getIdentifier() == variableName)
                {
                    return getDeclarations()[i];
                }
            }

            return nullptr;
        }

        vector<Expression *> &getErrors() { return _errors; }
        vector<StructType *> &getTypeErrors() { return _typeErrors; }
    };
} // namespace weasel
