#include "weasel/Parser/Parser.h"

ExpressionHandle Parser::parseStatement()
{
    LOG(INFO) << "Parse Statement...";

    // return make_shared<ErrorExpression>(getCurrentToken(), Errors::getInstance().unimplementedSyntax.withToken(getCurrentToken()));
    // Compound Statement Expression
    if (getCurrentToken().isOpenCurly())
    {
        return parseCompoundStatement();
    }

    // // If Statement
    // if (getCurrentToken().isKeyIf())
    // {
    //     return parseConditionStatement();
    // }

    // // For Statement
    // if (getCurrentToken().isKeyFor())
    // {
    //     return parseLoopingStatement();
    // }

    // // Variable Definition Expression
    // if (getCurrentToken().isKeyDefinition())
    // {
    //     return parseDeclarationExpression();
    // }

    // // Return Expression
    // if (getCurrentToken().isKeyReturn())
    // {
    //     return parseReturnExpression();
    // }

    // // Break Expression
    // if (getCurrentToken().isKeyBreak())
    // {
    //     return parseBreakExpression();
    // }

    // // Continue Expression
    // if (getCurrentToken().isKeyContinue())
    // {
    //     return parseContinueExpression();
    // }

    auto expr = parseExpression();
    if (expr->isError())
    {
        skipUntilNewLine();
    }

    return expr;
}

StructTypeHandle Parser::parseStruct()
{
    auto structType = make_shared<StructType>();
    if (!getNextToken().isIdentifier())
    {
        structType->setError(Errors::getInstance().expectedIdentifier.withToken(getCurrentToken()));
        return structType;
    }

    auto tokenIndentifier = getCurrentToken();
    if (!getNextToken().isOpenCurly())
    {
        structType->setError(Errors::getInstance().expectedOpenCurly.withToken(getCurrentToken()));
        return structType;
    }

    getNextToken(true); // eat '{' and '\n'

    structType->setIdentifier(tokenIndentifier.getValue());
    while (!getCurrentToken().isCloseCurly())
    {
        if (!getCurrentToken().isIdentifier())
        {
            structType->setError(Errors::getInstance().expectedIdentifier.withToken(getCurrentToken()));
            return structType;
        }

        auto identToken = getCurrentToken();
        getNextToken(); // eat 'identifier'
        if (!(getCurrentToken().isDataType() || getCurrentToken().isIdentifier()))
        {
            structType->setError(Errors::getInstance().expectedDataType.withToken(getCurrentToken()));
            return structType;
        }

        auto propType = parseDataType();
        structType->getFields().push_back(StructTypeField(identToken, identToken.getValue(), propType));
        if (!getCurrentToken().isNewline())
        {
            structType->setError(Errors::getInstance().expectedNewLine.withToken(getCurrentToken()));
            return structType;
        }

        getNextToken(); // eat '\n'
    }

    getNextToken(); // eat '}'

    return structType;
}

// weasel::Expression *weasel::Parser::parseLoopingStatement()
// {
//     return nullptr;

//     // auto token = getCurrentToken();
//     // auto isInfinity = getNextToken(true).isOpenCurly();
//     // auto conditions = std::vector<Expression *>();

//     // // Check if Infinity Looping
//     // if (!isInfinity)
//     // {
//     //     // Initial or Definition
//     //     if (!getCurrentToken().isSemiColon())
//     //     {
//     //         Expression *decl;
//     //         if (getCurrentToken().isKeyDeclaration())
//     //         {
//     //             decl = parseDeclarationExpression();
//     //         }
//     //         else
//     //         {
//     //             decl = parseExpression();
//     //         }

//     //         conditions.push_back(decl);
//     //     }

//     //     // Break if looping is Loop Condition
//     //     if (!getCurrentToken().isOpenCurly())
//     //     {
//     //         // Condition
//     //         if (!getCurrentToken().isSemiColon())
//     //         {
//     //             return ErrorTable::addError(getCurrentToken(), "Invalid Loop condition expression");
//     //         }

//     //         if (getNextToken().isSemiColon())
//     //         {
//     //             conditions.push_back(nullptr);
//     //         }
//     //         else
//     //         {
//     //             conditions.push_back(parseExpression());
//     //         }

//     //         // Increment
//     //         if (!getCurrentToken().isSemiColon())
//     //         {
//     //             return ErrorTable::addError(getCurrentToken(), "Invalid Loop counting expression");
//     //         }
//     //         if (getNextToken().isOpenCurly())
//     //         {
//     //             conditions.push_back(nullptr);
//     //         }
//     //         else
//     //         {
//     //             conditions.push_back(parseExpression());
//     //         }
//     //     }
//     // }

//     // if (!getCurrentToken().isOpenCurly())
//     // {
//     //     return ErrorTable::addError(getCurrentToken(), "Invalid Loop body statement");
//     // }

//     // // Check if All Conditions is empty
//     // if (!isInfinity)
//     // {
//     //     isInfinity = true;

//     //     for (auto &item : conditions)
//     //     {
//     //         if (item != nullptr)
//     //         {
//     //             isInfinity = false;
//     //             break;
//     //         }
//     //     }

//     //     if (isInfinity)
//     //     {
//     //         conditions.clear();
//     //     }
//     // }

//     // auto body = parseCompoundStatement();

//     // return new LoopingStatement(token, conditions, body);
// }

// weasel::Expression *weasel::Parser::parseConditionStatement()
// {
//     return nullptr;

//     // auto conditions = std::vector<Expression *>();
//     // auto stmts = std::vector<CompoundStatement *>();
//     // auto token = getCurrentToken();

//     // while (true)
//     // {
//     //     auto isElseCondition = true;
//     //     if (getCurrentToken().isKeyIf())
//     //     {
//     //         getNextToken(); // eat 'if'

//     //         isElseCondition = false;
//     //     }

//     //     if (!isElseCondition)
//     //     {
//     //         auto expr = parseExpression();
//     //         if (expr == nullptr)
//     //         {
//     //             auto errToken = getCurrentToken();

//     //             getNextTokenUntil(TokenKind::TokenSpaceNewline);
//     //             return ErrorTable::addError(errToken, "Invalid condition expression");
//     //         }

//     //         conditions.push_back(expr);
//     //     }

//     //     auto body = parseCompoundStatement();
//     //     if (body == nullptr)
//     //     {
//     //         auto errToken = getCurrentToken();

//     //         getNextTokenUntil(TokenKind::TokenSpaceNewline);
//     //         return ErrorTable::addError(errToken, "Invalid if body statement expression");
//     //     }

//     //     stmts.push_back(body);

//     //     if (isElseCondition)
//     //     {
//     //         break;
//     //     }

//     //     if (getCurrentToken().isKeyElse())
//     //     {
//     //         getNextToken(true); // eat 'else'
//     //         continue;
//     //     }

//     //     if (isExpectElse())
//     //     {
//     //         getNextToken(true); // eat
//     //         getNextToken(true); // eat 'else'
//     //         continue;
//     //     }

//     //     break;
//     // }

//     // return new ConditionStatement(token, conditions, stmts);
// }

CompoundStatementHandle Parser::parseCompoundStatement()
{
    LOG(INFO) << "Parse Compound Statement...";

    auto stmt = make_shared<CompoundStatement>();
    if (!getCurrentToken().isKind(TokenKind::TokenDelimOpenCurlyBracket))
    {
        stmt->setError(Errors::getInstance().expectedOpenCurly.withToken(getCurrentToken()));
        return stmt;
    }

    if (getNextToken(true).isKind(TokenKind::TokenDelimCloseCurlyBracket))
    {
        getNextToken(); // eat '}'
        return stmt;
    }

    do
    {
        auto expr = parseStatement();
        stmt->getBody().push_back(expr);

        if (expr->isError())
        {
            skipUntilNewLine();
        }

        if (getCurrentToken().isKind(TokenKind::TokenDelimCloseCurlyBracket))
        {
            break;
        }

        if (expr->isCompoundExpression() && !getCurrentToken().isNewline())
        {
            continue;
        }

        if (!getCurrentToken().isNewline())
        {
            return stmt;
        }

        getNextToken(true);
    } while (!getCurrentToken().isKind(TokenKind::TokenDelimCloseCurlyBracket));

    getNextToken(); // eat '}'

    return stmt;
}

// weasel::Expression *weasel::Parser::parseDeclarationExpression()
// {
//     return nullptr;

//     // auto qualifier = getQualifier();
//     // auto qualToken = getCurrentToken();

//     // // eat qualifier(let, final, const)
//     // if (!getNextToken().isIdentifier())
//     // {
//     //     auto errToken = getCurrentToken();

//     //     getNextTokenUntil(TokenKind::TokenSpaceNewline);
//     //     return ErrorTable::addError(errToken, "Expected an identifier");
//     // }

//     // auto identifier = getCurrentToken().getValue();

//     // getNextToken(); // eat 'identifier' and get next token

//     // auto type = parseDataType();
//     // if (getCurrentToken().isNewline())
//     // {
//     //     if (type == nullptr)
//     //     {
//     //         return ErrorTable::addError(getCurrentToken(), "Data Type Expected for default value declaration");
//     //     }

//     //     if (qualifier != Qualifier::QualVolatile)
//     //     {
//     //         return ErrorTable::addError(getCurrentToken(), "No Default Value for Non Volatile variable");
//     //     }

//     //     // Insert Symbol Table
//     //     addAttribute(ParserAttribute::get(identifier, type, AttributeKind::Variable));

//     //     // Create Variable with Default Value
//     //     return new DeclarationStatement(qualToken, identifier, qualifier, type);
//     // }

//     // // Equal
//     // if (!getCurrentToken().isOperatorEqual())
//     // {
//     //     auto errToken = getCurrentToken();

//     //     getNextTokenUntil(TokenKind::TokenSpaceNewline);
//     //     return ErrorTable::addError(errToken, "Expected equal sign");
//     // }

//     // // Get Next Value
//     // if (getNextToken().isNewline())
//     // {
//     //     return ErrorTable::addError(getCurrentToken(), "Expected RHS Value Expression but got 'New line'");
//     // }

//     // auto val = parseExpression();
//     // if (!val)
//     // {
//     //     auto errToken = getCurrentToken();

//     //     getNextTokenUntil(TokenKind::TokenSpaceNewline);
//     //     return ErrorTable::addError(errToken, "Expected RHS Value Expression but got not valid expression");
//     // }

//     // if (type == nullptr)
//     // {
//     //     if (val->getType() == nullptr)
//     //     {
//     //         return ErrorTable::addError(val->getToken(), "Cannot detect declaration and value type");
//     //     }

//     //     type = val->getType();
//     // }

//     // auto declExpr = new DeclarationStatement(qualToken, identifier, qualifier, type, val);
//     // if (getCurrentToken().isOpenCurly())
//     // {
//     //     enterScope();

//     //     // Insert Symbol Table
//     //     addAttribute(ParserAttribute::get(identifier, type, AttributeKind::Variable));

//     //     // Insert Symbol Table
//     //     auto compound = parseCompoundStatement();

//     //     exitScope();

//     //     if (compound != nullptr)
//     //     {
//     //         compound->insertBody(0, declExpr);

//     //         return compound;
//     //     }
//     // }

//     // // Insert Symbol Table
//     // addAttribute(ParserAttribute::get(identifier, type, AttributeKind::Variable));

//     // return declExpr;
// }
