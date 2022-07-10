#include <iostream>
#include <llvm/IR/Type.h>
#include "weasel/Parser/Parser.h"
#include "weasel/IR/Context.h"
#include "weasel/Symbol/Symbol.h"

// weasel::Function *weasel::Parser::parsePrallelFunction()
// {
//     auto parallelType = ParallelType::ParallelFunction;
//     if (getNextToken(true)->isKind(TokenKind::TokenKeyKernel))
//     {
//         parallelType = ParallelType::ParallelKernel;

//         getNextToken(true); // eat kernel
//     }

//     return parseFunction(parallelType);
// }

weasel::Function *weasel::Parser::parseFunction()
{
    auto fun = parseDeclareFunction();
    if (fun == nullptr)
    {
        return nullptr;
    }

    // Ignore new line
    if (getCurrentToken().isKind(TokenKind::TokenSpaceNewline))
    {
        getNextToken(true);
    }

    if (!getCurrentToken().isKind(TokenKind::TokenDelimOpenCurlyBracket))
    {
        return ErrorTable::addError(getCurrentToken(), "Expected {");
    }

    // Set Symbol for parameters and enter a scope
    {
        SymbolTable::enterScope();
        for (const auto &arg : fun->getArgs())
        {
            auto argName = arg->getArgumentName();
            auto ty = arg->getArgumentType();

            auto attrKind = AttributeKind::SymbolVariable;
            if (ty->isPointerType())
            {
                attrKind = AttributeKind::SymbolPointer;
            }
            else if (ty->isArrayType())
            {
                attrKind = AttributeKind::SymbolArray;
            }

            auto attr = new Attribute(argName, AttributeScope::ScopeParam, attrKind, ty);

            SymbolTable::insert(argName, attr);
        }
    }

    getNextToken(true); // eat {

    auto body = parseFunctionBody();

    // Exit parameter scope
    {
        SymbolTable::exitScope();
        SymbolTable::exitScope();
    }

    if (!body)
    {
        return ErrorTable::addError(getCurrentToken(), "Expected valid body statement!.");
    }

    if (!body->getBody().empty())
    {
        fun->setIsDefine(true);
    }

    fun->setBody(body);

    return fun;
}

// extern 'fun' identifier '(' args ')' funTy
weasel::Function *weasel::Parser::parseDeclareFunction()
{
    // get next and eat 'fun'
    if (!getNextToken().isKind(TokenKind::TokenIdentifier))
    {
        return ErrorTable::addError(getCurrentToken(), "Expected an identifier");
    }

    // Check Symbol Table
    auto identifier = getCurrentToken().getValue();
    if (SymbolTable::get(identifier) != nullptr)
    {
        return ErrorTable::addError(getCurrentToken(), "Function already declared");
    }

    if (!getNextToken().isKind(TokenKind::TokenDelimOpenParen))
    {
        return ErrorTable::addError(getCurrentToken(), "Expected (");
    }

    getNextToken(); // eat '('
    std::vector<weasel::FunctionArgument *> args;
    auto isVararg = false;

    while (!getCurrentToken().isKind(TokenKind::TokenDelimCloseParen))
    {
        if (isVararg)
        {
            return ErrorTable::addError(getCurrentToken(), "Variable number argument should be final argument");
        }

        auto idenToken = getCurrentToken();
        if (!getCurrentToken().isKind(TokenKind::TokenIdentifier))
        {
            return ErrorTable::addError(getCurrentToken(), "Expected identifier in function argument");
        }

        auto identifier = getCurrentToken().getValue();
        if (getNextToken().isKind(TokenKind::TokenPuncDotThree))
        {
            isVararg = true;
            getNextToken(); // eat ...
        }

        auto type = parseDataType();
        if (type == nullptr)
        {
            return ErrorTable::addError(getCurrentToken(), "Expected type in function argument");
        }

        args.push_back(new FunctionArgument(idenToken, identifier, type));

        if (!getNextToken().isKind(TokenKind::TokenPuncComma))
        {
            break;
        }

        getNextToken(); // eat ','
    }

    if (!getCurrentToken().isKind(TokenKind::TokenDelimCloseParen))
    {
        return ErrorTable::addError(getCurrentToken(), "Expected ) in function argument");
    }

    getNextToken(); // eat )

    auto returnType = parseDataType();
    if (returnType == nullptr)
    {
        returnType = Type::getVoidType();
    }

    auto funTy = new FunctionType(returnType, args, isVararg);
    auto fun = new Function(identifier, funTy);

    // Create Symbol for the function
    {
        SymbolTable::insert(identifier, new Attribute(identifier, AttributeScope::ScopeGlobal, AttributeKind::SymbolFunction, returnType));
    }

    return fun;
}
