#include "weasel/Parser/Parser.h"

ExpressionHandle Parser::parseStatement()
{
    LOG(INFO) << "Parse Statement...";

    // Compound Statement Expression
    if (getCurrentToken().isOpenCurly())
    {
        return parseCompoundStatement();
    }

    // If Statement
    if (getCurrentToken().isKeyIf())
    {
        return parseConditionStatement();
    }

    // // For Statement
    // if (getCurrentToken().isKeyFor())
    // {
    //     return parseLoopingStatement();
    // }

    // Variable Definition Expression
    if (getCurrentToken().isKeyDefinition())
    {
        return parseDeclarationExpression();
    }

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
//     // auto conditions = vector<Expression *>();

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

ExpressionHandle Parser::parseConditionStatement()
{
    auto stmt = make_shared<ConditionStatement>();
    while (true)
    {
        auto isElseCondition = true;
        if (getCurrentToken().isKeyIf())
        {
            getNextToken(); // eat 'if'

            isElseCondition = false;
        }

        if (!isElseCondition)
        {
            auto expr = parseExpression();
            stmt->getConditions().push_back(expr);
            if (expr->isError())
            {
                return expr;
            }
        }

        if (!getCurrentToken().isOpenCurly())
        {
            stmt->setError(Errors::getInstance().expectedOpenCurly.withToken(getCurrentToken()));
            return stmt;
        }

        auto body = parseCompoundStatement();
        stmt->getStatements().push_back(body);
        if (body->isError())
        {
            return stmt;
        }

        if (isElseCondition)
        {
            break;
        }

        if (getCurrentToken().isKeyElse())
        {
            getNextToken(true); // eat 'else'
            continue;
        }

        if (isExpectElse())
        {
            getNextToken(true); // eat
            getNextToken(true); // eat 'else'
            continue;
        }

        break;
    }

    return stmt;
}

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

ExpressionHandle Parser::parseDeclarationExpression()
{
    auto qualifier = getQualifier();
    auto stmt = make_shared<DeclarationStatement>();

    // eat qualifier(let, final, const)
    if (!getNextToken().isIdentifier())
    {
        stmt->setError(Errors::getInstance().expectedIdentifier.withToken(getCurrentToken()));
        return stmt;
    }

    stmt->setQualifier(qualifier);
    stmt->setToken(getCurrentToken());
    stmt->setIdentifier(getCurrentToken().getValue());

    getNextToken(); // eat 'identifier'
    if (getCurrentToken().isDataType() || getCurrentToken().isIdentifier())
    {
        stmt->setType(parseDataType());
    }

    if (getCurrentToken().isNewline())
    {
        if (!stmt->getType())
        {
            stmt->setError(Errors::getInstance().expectedDefaultValue.withToken(getCurrentToken()));
        }

        return stmt;
    }

    // Equal
    if (!getCurrentToken().isOperatorEqual())
    {
        stmt->setError(Errors::getInstance().expectedEqualSign.withToken(getCurrentToken()));
        return stmt;
    }

    // Get Next Value
    if (getNextToken().isNewline())
    {
        stmt->setError(Errors::getInstance().expectedRHSValue.withToken(getCurrentToken()));
        return stmt;
    }

    auto val = parseExpression();
    stmt->setValue(val);

    if (getCurrentToken().isOpenCurly())
    {
        // Insert Symbol Table
        auto compound = parseCompoundStatement();
        compound->getBody().insert(compound->getBody().begin(), stmt);

        return compound;
    }

    return stmt;
}
