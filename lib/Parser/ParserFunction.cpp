#include "weasel/Parser/Parser.h"
#include "weasel/Symbol/Symbol.h"

weasel::Function *weasel::Parser::parseExternFunction()
{
    getNextToken(true); // eat 'extern'
    auto fun = parseFunction();
    fun->setIsExtern(true);

    return fun;
}

weasel::Function *weasel::Parser::parseFunction(StructType *type)
{
    auto fun = parseDeclareFunction(type);
    if (fun == nullptr)
    {
        return nullptr;
    }

    // Set Impl Struct Type
    fun->setImplStruct(type);

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
        return ErrorTable::addError(getCurrentToken(), "Expected an identifier when parse function");
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
    auto isStatic = implType != nullptr;
    while (!getCurrentToken().isCloseParen())
    {
        if (isVararg)
        {
            return ErrorTable::addError(getCurrentToken(), "Variable number argument should be final argument");
        }

        auto lastToken = getCurrentToken();
        if (lastToken.isKeyThis())
        {
            // Check if using reference type
            Type *type = implType;
            if (getNextToken().isOperatorAnd())
            {
                type = Type::getReferenceType(implType);
                getNextToken(); // eat '&'
            }

            types.push_back(ArgumentType::create(lastToken.getValue(), type));
            isStatic = false;
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

    auto newFunction = new Function(identifier, returnType, types);
    newFunction->setIsStatic(isStatic);
    return newFunction;
}

void weasel::Parser::parseImplFunctions()
{
    auto structName = getNextToken().getValue();
    auto structType = _module->findStructType(structName);

    getNextToken();     // eat StructName
    getNextToken(true); // eat '{'

    while (!getCurrentToken().isCloseCurly())
    {
        addFunction(parseFunction(structType));

        if (getCurrentToken().isNewline())
        {
            getNextToken(true);
        }
    }
}
