#pragma once

#include <list>

#include <weasel/AST/AST.h>
#include <weasel/Lexer/Lexer.h>
#include <weasel/Type/Type.h>
#include <weasel/Table/ParserTable.h>
#include <weasel/IR/Module.h>

using namespace std;
using namespace weasel;

namespace weasel
{
    class ParserAttribute;

    class Parser
    {
    public:
        Parser(const Lexer &lexer, Module *module) : _lexer(lexer), _module(module) {}

        // Helper
        TypeHandle parseDataType();
        void ignoreNewline();

        // Lexer
        Lexer getLexer() const { return _lexer; }

        // Parse
        void parse();

        // Module
        Module *getModule() const { return _module; }

    private:
        // Simple Class for accessing token
        Lexer _lexer;

        // Simple Class for handling and storing global value
        Module *_module;

        Token expectToken() { return _lexer.expect(); }
        bool expectToken(TokenKind kind) { return _lexer.expect(kind); }
        bool isExpectElse() { return expectToken(TokenKind::TokenKeyElse); }

        // Parser Helper
        Qualifier getQualifier() const { return getCurrentToken().getQualifier(); }
        Token getCurrentToken() const { return _lexer.getCurrentToken(); }
        Token getNextToken(bool skipSpace = false);
        Token getNextTokenUntil(TokenKind kind);
        Token skipUntilNewLine() { return getNextTokenUntil(TokenKind::TokenSpaceNewline); }
        bool isDataType();

        // Operator Expression
        ExpressionHandle createOperatorExpression(Token op, ExpressionHandle lhs, ExpressionHandle rhs);

        // Impl Functions
        void parseImplFunctions();

        // Global Variable
        GlobalVariableHandle parseGlobalVariable();

        // Function
        FunctionHandle parseDeclareFunction();
        FunctionHandle parseFunction();

        // Parse Global Type
        StructTypeHandle parseStruct();

        // Statement
        ExpressionHandle parseStatement();
        CompoundStatementHandle parseCompoundStatement();
        ExpressionHandle parseConditionStatement();
        ExpressionHandle parseLoopingStatement();
        ExpressionHandle parseStaticMethodCallExpression();
        // Expression *parseMethodCallExpression(Expression *);

        // Expression
        ExpressionHandle parseExpression();
        ExpressionHandle parsePrimaryExpression();
        ExpressionHandle parseDeclarationExpression();
        ExpressionHandle parseCallExpression();
        ExpressionHandle parseParenExpression();
        ExpressionHandle parseUnaryExpression();
        ExpressionHandle parseReturnExpression();
        ExpressionHandle parseBreakExpression();
        ExpressionHandle parseContinueExpression();
        ExpressionHandle parseStructExpression();
        ExpressionHandle parseFieldExpression(ExpressionHandle lhs);

        // Expression Literal
        ExpressionHandle parseLiteralExpression();
        ExpressionHandle parseIdentifierExpression();
        ExpressionHandle parseExpressionOperator(unsigned prec, ExpressionHandle lhs);
        // Expression *parseArrayExpression();
    };
} // namespace weasel
