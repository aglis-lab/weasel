#include <cassert>

#include "weasel/Parser/Parser.h"

FunctionHandle Parser::parseFunction()
{
    LOG(INFO) << "Parse Function...";

    auto fun = parseDeclareFunction();
    if (fun->isError())
    {
        return fun;
    }

    if (getCurrentToken().isNewline())
    {
        return fun;
    }

    if (!getCurrentToken().isOpenCurly())
    {
        fun->setError(Errors::getInstance().expectedOpenCurly.withToken(getCurrentToken()));
        return fun;
    }

    auto body = parseCompoundStatement();
    if (body && !body->getBody().empty())
    {
        fun->setIsDefine(true);
    }

    fun->setBody(body);

    return fun;
}

// 'fun' identifier '(' args ')' funTy
FunctionHandle Parser::parseDeclareFunction()
{
    LOG(INFO) << "Parse Declare Function...";

    auto fun = make_shared<Function>();

    // get next and eat 'fun'
    if (!getNextToken().isIdentifier())
    {
        fun->setError(Errors::getInstance().expectedIdentifier.withToken(getCurrentToken()));
        return fun;
    }

    // Check Symbol Table
    auto identToken = getCurrentToken();
    fun->setToken(identToken);
    fun->setIdentifier(identToken.getValue());

    if (!getNextToken().isOpenParen())
    {
        fun->setError(Errors::getInstance().expectedOpenParen.withToken(getCurrentToken()));
        return fun;
    }

    getNextToken(); // eat '('
    auto isVararg = false;
    while (!getCurrentToken().isCloseParen())
    {
        if (isVararg)
        {
            fun->setError(Errors::getInstance().invalidVararg.withToken(getCurrentToken()));
            return fun;
        }

        auto lastToken = getCurrentToken();
        if (lastToken.isKeyThis())
        {
            auto argumentType = make_shared<ArgumentExpression>();

            if (getNextToken().isOperatorAnd())
            {
                argumentType->setImplThis(ArgumentExpression::Reference);
                getNextToken(); // eat '&'
            }
            else
            {
                argumentType->setImplThis(ArgumentExpression::Value);
            }

            argumentType->setToken(lastToken);
            argumentType->setIdentifier(lastToken.getValue());
            fun->getArguments().push_back(argumentType);
        }
        else
        {
            if (!lastToken.isIdentifier())
            {
                fun->setError(Errors::getInstance().expectedIdentifier.withToken(getCurrentToken()));
                return fun;
            }

            if (getNextToken().isKind(TokenKind::TokenPuncDotThree))
            {
                isVararg = true;
                getNextToken(); // eat ...
            }

            auto type = parseDataType();
            auto argumentType = make_shared<ArgumentExpression>();

            argumentType->setType(type);
            argumentType->setIdentifier(lastToken.getValue());

            fun->getArguments().push_back(argumentType);
        }

        if (!getCurrentToken().isKind(TokenKind::TokenPuncComma))
        {
            break;
        }

        getNextToken(); // eat ','
    }

    if (!getCurrentToken().isCloseParen())
    {
        fun->setError(Errors::getInstance().expectedCloseParen.withToken(getCurrentToken()));
        return fun;
    }

    getNextToken(); // eat ')'

    auto returnType = Type::getVoidType();
    if (getCurrentToken().isDataType() || getCurrentToken().isIdentifier())
    {
        returnType = parseDataType();
    }

    if (isVararg)
    {
        fun->setVararg(true);
    }

    fun->setType(returnType);
    return fun;
}

// TODO: Do much better error checking
void Parser::parseImplFunctions()
{
    LOG(INFO) << "Parse Impl Functions";

    assert(getCurrentToken().isKeyImpl());

    getNextToken(); // eat 'impl'
    assert(getCurrentToken().isDataTypeSingleValue());

    auto implType = parseDataType();
    assert(getCurrentToken().isOpenCurly());

    getNextToken(true); // eat '{'
    while (!getCurrentToken().isCloseCurly())
    {
        auto fun = parseFunction();

        fun->setImplType(implType);
        getModule()->addFunction(fun);

        ignoreNewline();
    }
}
