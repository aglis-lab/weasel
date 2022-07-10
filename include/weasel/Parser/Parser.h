#pragma once

#include "weasel/AST/AST.h"
#include "weasel/Lexer/Lexer.h"
#include "weasel/Type/Type.h"

namespace weasel
{
    class Context;
    class Attribute;

    class Parser
    {
    private:
        Lexer *_lexer;

    private:
        bool expectToken(TokenKind kind) { return _lexer->expect(kind); }

        Qualifier getQualifier() const { return getCurrentToken().getQualifier(); }
        Token getCurrentToken() const { return _lexer->getCurrentToken(); }
        Token getNextToken(bool skipSpace = false);
        Token getNextTokenUntil(TokenKind kind);

        // Function
        Function *parseDeclareFunction();
        Function *parseFunction();

        // Statement
        StatementExpression *parseFunctionBody();
        Expression *parseStatement();
        Expression *parseCompoundStatement();
        Expression *parseReturnStatement();

        // Expression
        Expression *parseExpression();
        Expression *parsePrimaryExpression();
        Expression *parseDeclarationExpression();
        Expression *parseFunctionCallExpression(Attribute *attr);
        Expression *parseParenExpression();

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
        std::vector<Function *> parse();
    };

} // namespace weasel
