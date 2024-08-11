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

    // For Statement
    if (getCurrentToken().isKeyFor())
    {
        return parseLoopingStatement();
    }

    // Variable Definition Expression
    if (getCurrentToken().isKeyDefinition())
    {
        return parseDeclarationStatement();
    }

    // Return Expression
    if (getCurrentToken().isKeyReturn())
    {
        return parseReturnExpression();
    }

    // Break Expression
    if (getCurrentToken().isKeyBreak())
    {
        return parseBreakExpression();
    }

    // Continue Expression
    if (getCurrentToken().isKeyContinue())
    {
        return parseContinueExpression();
    }

    // Parse Expression
    return parseExpressionWithBlock();
}

StructTypeHandle Parser::parseStruct()
{
    LOG(INFO) << "Parse Struct...";

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
        if (!(getCurrentToken().isDataType() || getCurrentToken().isIdentifier() || getCurrentToken().isOpenSquare()))
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

ExpressionHandle Parser::parseLoopingStatement()
{
    LOG(INFO) << "Parse Looping Statement...";

    auto expr = make_shared<LoopingStatement>(getCurrentToken());
    auto isInfinity = getNextToken().isOpenCurly();

    // Check if Infinity Looping
    if (!isInfinity)
    {
        // Initial or Definition
        if (!getCurrentToken().isSemiColon())
        {
            ExpressionHandle decl;
            if (getCurrentToken().isKeyDeclaration())
            {
                decl = parseDeclarationStatement();
            }
            else
            {
                decl = parseExpressionWithBlock();
            }

            expr->getConditions().push_back(decl);
        }

        // Break if looping is Loop Condition
        if (!getCurrentToken().isOpenCurly())
        {
            // Condition
            if (!getCurrentToken().isSemiColon())
            {
                expr->setError(Errors::getInstance().expectedSemicolon.withToken(getCurrentToken()));
                return expr;
            }

            if (getNextToken().isSemiColon())
            {
                expr->getConditions().push_back(nullptr);
            }
            else
            {
                expr->getConditions().push_back(parseExpressionWithBlock());
            }

            // Increment
            if (!getCurrentToken().isSemiColon())
            {
                expr->setError(Errors::getInstance().expectedSemicolon.withToken(getCurrentToken()));
                return expr;
            }

            if (getNextToken().isOpenCurly())
            {
                expr->getConditions().push_back(nullptr);
            }
            else
            {
                expr->getConditions().push_back(parseExpressionWithBlock());
            }
        }
    }

    if (!getCurrentToken().isOpenCurly())
    {
        expr->setError(Errors::getInstance().expectedOpenCurly.withToken(getCurrentToken()));
        return expr;
    }

    // Check if All Conditions is empty
    if (!isInfinity)
    {
        isInfinity = true;

        for (auto item : expr->getConditions())
        {
            if (item != nullptr)
            {
                isInfinity = false;
                break;
            }
        }

        if (isInfinity)
        {
            expr->getConditions().clear();
        }
    }

    auto body = parseCompoundStatement();
    expr->setBody(body);

    return expr;
}

ExpressionHandle Parser::parseConditionStatement()
{
    LOG(INFO) << "Parse Condition Statement...";

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
            auto expr = parseExpressionWithoutBlock();
            stmt->getConditions().push_back(expr);
            if (expr->isError())
            {
                return stmt;
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
            getNextToken(); // eat 'else'
            continue;
        }

        if (expectToken().isKeyElse())
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

ExpressionHandle Parser::parseDeclarationStatement()
{
    LOG(INFO) << "Parse Declaration Expression";

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
    if (isDataType())
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

    auto val = parseExpressionWithBlock();
    stmt->setValue(val);

    if (getCurrentToken().isSemiColon() && expectToken().isOpenCurly())
    {
        // Insert Symbol Table
        auto compound = parseCompoundStatement();
        compound->getBody().insert(compound->getBody().begin(), stmt);

        return compound;
    }

    return stmt;
}

ExpressionHandle Parser::parseLambdaExpression()
{
    LOG(INFO) << "Parse Lambda Expression";
}
