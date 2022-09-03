#pragma once

#include <list>

#include "weasel/AST/AST.h"
#include "weasel/Lexer/Lexer.h"
#include "weasel/Type/Type.h"
#include "weasel/Table/ParserTable.h"

namespace weasel
{
    class ParserAttribute;

    class Parser : ParserTable
    {
    private:
        Lexer *_lexer;
        Function *_currentFunction;
        std::list<StructType *> _userTypes;
        std::list<Function *> _functions;

    private:
        inline void addFunction(Function *fun) { _functions.push_back(fun); }
        inline unsigned functionCount() const { return _functions.size(); }
        inline Function *lastFunction() const { return _functions.back(); }
        inline Function *findFunction(const std::string &identifier)
        {
            for (auto item : getFunctions())
            {
                if (item->getIdentifier() == identifier)
                {
                    return item;
                }
            }

            return nullptr;
        }

        inline void addUserType(StructType *type) { _userTypes.push_back(type); }
        inline unsigned userTypeCount() const { return _userTypes.size(); }
        inline StructType *getLastUserType() const { return _userTypes.back(); }
        inline StructType *findUserType(const std::string &typeName)
        {
            for (auto item : getUserTypes())
            {
                if (item->getIdentifier() == typeName)
                {
                    return item;
                }
            }

            return nullptr;
        }

    private:
        bool expectToken(TokenKind kind) { return _lexer->expect(kind); }
        bool isExpectElse() { return expectToken(TokenKind::TokenKeyElse); }

        // Parser Helper
        Qualifier getQualifier() const { return getCurrentToken().getQualifier(); }
        Token getCurrentToken() const { return _lexer->getCurrentToken(); }
        Token getNextToken(bool skipSpace = false);
        Token getNextTokenUntil(TokenKind kind);

        Expression *createOperatorExpression(Token op, Expression *lhs, Expression *rhs);

        // Function
        Function *parseDeclareFunction();
        Function *parseFunction();

        // Parse Global Type
        StructType *parseStruct();

        // Statement
        Expression *parseStatement();
        CompoundStatement *parseCompoundStatement();
        Expression *parseConditionStatement();
        Expression *parseLoopingStatement();

        // Expression
        Expression *parseExpression();
        Expression *parsePrimaryExpression();
        Expression *parseDeclarationExpression();
        Expression *parseCallExpression(Function *fun);
        Expression *parseParenExpression();
        Expression *parseReturnExpression();
        Expression *parseBreakExpression();
        Expression *parseContinueExpression();
        Expression *parseStructExpression();
        Expression *parseFieldExpression(Expression *lhs);

        // Expression Literal
        Expression *parseLiteralExpression();
        Expression *parseIdentifierExpression();
        Expression *parseExpressionOperator(unsigned prec, Expression *lhs);
        Expression *parseArrayExpression();

    public:
        Parser(Lexer *lexer) : _lexer(lexer) {}

        // Gets Parser Value
        inline std::list<StructType *> getUserTypes() const { return _userTypes; }
        inline std::list<Function *> getFunctions() const { return _functions; }

        // Helper
        Type *parseDataType();
        void ignoreNewline();

        // Lexer
        Lexer *getLexer() const { return _lexer; }

    public:
        void parse();
    };
} // namespace weasel
