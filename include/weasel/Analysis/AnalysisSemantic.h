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
        void semantic(Function *fun);
        void semantic(CompoundStatement *expr);
        void semantic(CallExpression *expr);
        void semantic(ConditionStatement *expr);
        void semantic(DeclarationStatement *expr);
        void semantic(VariableExpression *expr);
        void semantic(AssignmentExpression *expr);
        void semantic(ComparisonExpression *expr);
        void semantic(ReturnExpression *expr);
        void semantic(BreakExpression *expr);
        void semantic(ContinueExpression *expr);
        void semantic(LoopingStatement *expr);
        void semantic(ArithmeticExpression *expr);
        void semantic(UnaryExpression *expr);
        void semantic(StructExpression *expr);
        void semantic(FieldExpression *expr);
        void semantic(TypeCastExpression *expr);
        void semantic(ArgumentExpression *expr);
        void semantic(NilLiteralExpression *expr);
        void semantic(IndexExpression *expr);
        void semantic(ArrayExpression *expr);
        void semantic(MethodCallExpression *expr);

        void semantic(FunctionType *expr);
        void semantic(StructType *expr);
        void semantic(Type *expr);

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
