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

    class Parser : public ParserTable
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

    private:
        // void addFunction(FunctionHandle fun) { _module.addFunction(fun); }
        // unsigned functionCount() const { return getFunctions().size(); }
        // FunctionHandle lastFunction() const { return getFunctions().back(); }
        // FunctionHandle findFunction(const std::string &identifier, StructType *structType = nullptr, bool isStatic = false);

        // void addUserType(StructTypeHandle type) { _module.addUserType(type); }
        // unsigned userTypeCount() const { return getUserTypes().size(); }
        // StructTypeHandle getLastUserType() const { return getUserTypes().back(); }

        // TODO: Find User Type
        // StructType *findUserType(const std::string &typeName);

        // void addGlobalVariable(GlobalVariableHandle globalVar) { _module.addGlobalVariable(globalVar); }
        // GlobalVariable *findGlobalVariable(const std::string &globalName);

    private:
        bool expectToken(TokenKind kind) { return _lexer.expect(kind); }
        bool isExpectElse() { return expectToken(TokenKind::TokenKeyElse); }

        // Parser Helper
        Qualifier getQualifier() const { return getCurrentToken().getQualifier(); }
        Token getCurrentToken() const { return _lexer.getCurrentToken(); }
        Token getNextToken(bool skipSpace = false);
        Token getNextTokenUntil(TokenKind kind);
        Token skipUntilNewLine() { return getNextTokenUntil(TokenKind::TokenSpaceNewline); }

        // Operator Expression
        Expression *createOperatorExpression(Token op, Expression *lhs, Expression *rhs);

        // Impl Functions
        void parseImplFunctions();

        // Global Variable
        GlobalVariable *parseGlobalVariable();

        // Function
        FunctionHandle parseDeclareFunction();
        FunctionHandle parseFunction();

        // Parse Global Type
        StructTypeHandle parseStruct();

        // Statement
        ExpressionHandle parseStatement();
        CompoundStatementHandle parseCompoundStatement();
        // Expression *parseConditionStatement();
        // Expression *parseLoopingStatement();
        // ExpressionHandle parseStaticMethodCallExpression();
        // Expression *parseMethodCallExpression(Expression *);

        // Expression
        ExpressionHandle parseExpression();
        ExpressionHandle parsePrimaryExpression();
        // Expression *parseDeclarationExpression();
        ExpressionHandle parseCallExpression();
        // Expression *parseParenExpression();
        // Expression *parseReturnExpression();
        // Expression *parseBreakExpression();
        // Expression *parseContinueExpression();
        // Expression *parseStructExpression();
        // Expression *parseFieldExpression(Expression *lhs);

        // Expression Literal
        ExpressionHandle parseLiteralExpression();
        ExpressionHandle parseIdentifierExpression();
        ExpressionHandle parseExpressionOperator(unsigned prec, ExpressionHandle lhs);
        // Expression *parseArrayExpression();
    };
} // namespace weasel
