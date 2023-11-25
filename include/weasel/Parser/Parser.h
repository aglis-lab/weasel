#pragma once

#include <list>

#include <weasel/AST/AST.h>
#include <weasel/Lexer/Lexer.h>
#include <weasel/Type/Type.h>
#include <weasel/Table/ParserTable.h>
#include <weasel/IR/Module.h>

namespace weasel
{
    class ParserAttribute;

    class Parser : ParserTable
    {
    public:
        Parser(Lexer *lexer, Module *module) : _lexer(lexer), _module(module) {}

        // Gets Parser Value
        std::vector<GlobalVariable *> getGlobalVariables() const { return _module->getGlobalVariables(); }
        std::vector<StructType *> getUserTypes() const { return _module->getUserTypes(); }
        std::vector<Function *> getFunctions() const { return _module->getFunctions(); }

        // Helper
        Type *parseDataType();
        void ignoreNewline();

        // Lexer
        Lexer *getLexer() const { return _lexer; }

    public:
        void parse();

    private:
        // Simple Class for accessing token
        Lexer *_lexer;

        // Simple Class for handling and storing global value
        Module *_module;

    private:
        void addFunction(Function *fun) { _module->addFunction(fun); }
        unsigned functionCount() const { return getFunctions().size(); }
        Function *lastFunction() const { return getFunctions().back(); }
        Function *findFunction(const std::string &identifier, StructType *structType = nullptr, bool isStatic = false);

        void addUserType(StructType *type) { _module->addUserType(type); }
        unsigned userTypeCount() const { return getUserTypes().size(); }
        StructType *getLastUserType() const { return getUserTypes().back(); }
        StructType *findUserType(const std::string &typeName);

        void addGlobalVariable(GlobalVariable *globalVar) { _module->addGlobalVariable(globalVar); }
        // GlobalVariable *findGlobalVariable(const std::string &globalName);

    private:
        bool expectToken(TokenKind kind) { return _lexer->expect(kind); }
        bool isExpectElse() { return expectToken(TokenKind::TokenKeyElse); }

        // Parser Helper
        Qualifier getQualifier() const { return getCurrentToken().getQualifier(); }
        Token getCurrentToken() const { return _lexer->getCurrentToken(); }
        Token getNextToken(bool skipSpace = false);
        Token getNextTokenUntil(TokenKind kind);

        Expression *createOperatorExpression(Token op, Expression *lhs, Expression *rhs);

        // Impl Functions
        void parseImplFunctions();

        // Global Variable
        GlobalVariable *parseGlobalVariable();

        // Function
        Function *parseDeclareFunction(StructType *type);
        Function *parseFunction(StructType *type = nullptr);
        Function *parseExternFunction();

        // Parse Global Type
        StructType *parseStruct();

        // Statement
        Expression *parseStatement();
        CompoundStatement *parseCompoundStatement();
        Expression *parseConditionStatement();
        Expression *parseLoopingStatement();
        Expression *parseStaticMethodCallExpression(StructType *structType);
        Expression *parseMethodCallExpression(Expression *);

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
    };
} // namespace weasel
