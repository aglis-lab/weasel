#include <iostream>
#include <llvm/IR/Type.h>
#include "weasel/Parser/Parser.h"
#include "weasel/IR/Context.h"
#include "weasel/Symbol/Symbol.h"

weasel::Function *weasel::Parser::parseFunction(StructType *type)
{
    auto fun = parseDeclareFunction(type);
    if (fun == nullptr)
    {
        return nullptr;
    }

    // Ignore new line
    if (getCurrentToken().isNewline())
    {
        getNextToken(true);
    }

    if (!getCurrentToken().isOpenCurly())
    {
        return ErrorTable::addError(getCurrentToken(), "Expected {");
    }

    // Set Symbol for parameters and enter a scope
    enterScope();

    for (auto &arg : fun->getArguments())
    {
        addAttribute(ParserAttribute::get(arg->getArgumentName(), arg->getType(), AttributeKind::Parameter));
    }

    auto body = parseCompoundStatement();

    // Exit parameter scope
    exitScope();

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

// 'fun' identifier '(' args ')' funTy
weasel::Function *weasel::Parser::parseDeclareFunction(StructType *implType)
{
    // get next and eat 'fun'
    if (!getNextToken().isIdentifier())
    {
        return ErrorTable::addError(getCurrentToken(), "Expected an identifier");
    }

    // Check Symbol Table
    auto identifier = getCurrentToken().getValue();
    if (findFunction(identifier) != nullptr)
    {
        return ErrorTable::addError(getCurrentToken(), "Function already declared");
    }

    if (!getNextToken().isOpenParen())
    {
        return ErrorTable::addError(getCurrentToken(), "Expected (");
    }

    getNextToken(); // eat '('
    std::vector<ArgumentType *> types;
    auto isVararg = false;
    while (!getCurrentToken().isCloseParen())
    {
        if (isVararg)
        {
            return ErrorTable::addError(getCurrentToken(), "Variable number argument should be final argument");
        }

        auto lastToken = getCurrentToken();
        if (lastToken.isKeyThis())
        {
            auto argumentType = ArgumentType::create(lastToken.getValue(), implType);

            types.push_back(argumentType);

            getNextToken(); // eat 'this'
        }
        else
        {
            if (!lastToken.isIdentifier())
            {
                return ErrorTable::addError(getCurrentToken(), "Expected identifier in function argument");
            }

            auto identifier = lastToken.getValue();
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

            auto argumentType = ArgumentType::create(identifier, type);

            types.push_back(argumentType);
        }

        if (!getCurrentToken().isKind(TokenKind::TokenPuncComma))
        {
            break;
        }

        getNextToken(); // eat ','
    }

    if (!getCurrentToken().isCloseParen())
    {
        return ErrorTable::addError(getCurrentToken(), "Expected ) in function argument");
    }

    getNextToken(); // eat )

    auto returnType = parseDataType();
    if (returnType == nullptr)
    {
        returnType = Type::getVoidType();
    }

    returnType->setSpread(isVararg);

    return new Function(identifier, returnType, types);
}
