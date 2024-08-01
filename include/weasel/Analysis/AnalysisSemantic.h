#pragma once

#include "weasel/Type/Type.h"
#include "weasel/Parser/Parser.h"

namespace weasel
{
    class AnalysisSemantic
    {
    private:
        Module *_module;

        vector<Expression *> _errors;
        vector<StructType *> _typeErrors;

        vector<DeclarationStatement *> _declarations;

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

        void semantic(FunctionType *expr);
        void semantic(StructType *expr);
        void semantic(Type *expr);

        void unknownType(Expression *expr);

        void onError(Expression *expr)
        {
            getErrors().push_back(expr);
        }

        void onStructError(StructType *expr)
        {
            getTypeErrors().push_back(expr);
        }

        Module *getModule() const { return _module; }
        vector<DeclarationStatement *> &getDeclarations() { return _declarations; }
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
