#pragma once

#include "weasel/AST/AST.h"
#include "weasel/Lexer/Lexer.h"
#include "weasel/Type/Type.h"

namespace weasel
{
    class Attribute;

    class Parser
    {
    private:
        Lexer *_lexer;
        Function *_currentFunction;

    private:
        bool expectToken(TokenKind kind) { return _lexer->expect(kind); }
        bool isExpectElse() { return expectToken(TokenKind::TokenKeyElse); }

        Qualifier getQualifier() const { return getCurrentToken().getQualifier(); }
        Token getCurrentToken() const { return _lexer->getCurrentToken(); }
        Token getNextToken(bool skipSpace = false);
        Token getNextTokenUntil(TokenKind kind);

        // Function
        Function *parseDeclareFunction();
        Function *parseFunction();

        // Struct
        GlobalObject *parseStruct();

        // Statement
        Expression *parseStatement();
        StatementExpression *parseCompoundStatement();
        Expression *parseConditionStatement();
        Expression *parseLoopingStatement();

        // Expression
        Expression *parseExpression();
        Expression *parsePrimaryExpression();
        Expression *parseDeclarationExpression();
        Expression *parseFunctionCallExpression(Attribute *attr);
        Expression *parseParenExpression();
        Expression *parseReturnExpression();
        Expression *parseBreakExpression();
        Expression *parseContinueExpression();

        // Expression Literal
        Expression *parseLiteralExpression();
        Expression *parseIdentifierExpression();
        Expression *parseBinaryOperator(unsigned prec, weasel::Expression *lhs);
        Expression *parseArrayExpression();

    public:
        Parser(Lexer *lexer) : _lexer(lexer) {}

        // Helper
        Type *parseDataType();

        void ignoreNewline();

    public:
        std::vector<GlobalObject *> parse();
    };

} // namespace weasel
