#include "weasel/Lexer/Token.h"
#include "weasel/AST/AST.h"

Token Token::create()
{
    return Token();
}

Token Token::create(TokenKind kind, char *startToken, char *endToken)
{
    return Token(kind, startToken, endToken);
}

Qualifier Token::getQualifier() const
{
    switch (getTokenKind())
    {
    case TokenKind::TokenKeyConst:
        return Qualifier::QualConst;
    case TokenKind::TokenKeyFinal:
        return Qualifier::QualRestrict;
    default:
        return Qualifier::QualVolatile;
    }
}

// TODO: Need to add associativity
Precedence Token::getPrecedence()
{
    Precedence val{};
    val.associative = Associative::LeftToRight;

    switch (_kind)
    {
    case TokenKind::TokenPuncDot:
    case TokenKind::TokenPuncDotThree:
    case TokenKind::TokenDelimOpenSquareBracket:
    case TokenKind::TokenDelimOpenParen:
    case TokenKind::TokenOperatorCasting:
        val.order = 2;
        break;
    case TokenKind::TokenOperatorStar:
    case TokenKind::TokenOperatorSlash:
    case TokenKind::TokenOperatorPercent:
        val.order = 5;
        break;
    case TokenKind::TokenOperatorNegative:
    case TokenKind::TokenOperatorPlus:
        val.order = 6;
        break;
    case TokenKind::TokenOperatorShiftLeft:
    case TokenKind::TokenOperatorShiftRight:
        val.order = 7;
        break;
    case TokenKind::TokenOperatorLessThan:
    case TokenKind::TokenOperatorLessEqual:
    case TokenKind::TokenOperatorGreaterThen:
    case TokenKind::TokenOperatorGreaterEqual:
        val.order = 9;
        break;
    case TokenKind::TokenOperatorEqualEqual:
    case TokenKind::TokenOperatorNotEqual:
        val.order = 10;
        break;
    case TokenKind::TokenOperatorOr:
    case TokenKind::TokenOperatorAnd:
        val.order = 11;
        break;
    case TokenKind::TokenOperatorCaret:
        val.order = 12;
        break;
    case TokenKind::TokenOperatorNot:
        val.order = 13;
        break;
    case TokenKind::TokenOperatorAndAnd:
        val.order = 14;
        break;
    case TokenKind::TokenOperatorOror:
        val.order = 15;
        break;
    case TokenKind::TokenOperatorEqual:
    case TokenKind::TokenOperatorPlusEqual:
    case TokenKind::TokenOperatorNegativeEqual:
    case TokenKind::TokenOperatorStarEqual:
    case TokenKind::TokenOperatorSlashEqual:
    case TokenKind::TokenOperatorPercentEqual:
    case TokenKind::TokenOperatorShiftLeftEqual:
    case TokenKind::TokenOperatorShiftRightEqual:
    case TokenKind::TokenOperatorAndEqual:
    case TokenKind::TokenOperatorCaretEqual:
        // case TokenKind::TokenOperatorNotEqual:
        val.associative = Associative::RightToLeft;
        val.order = 16;
        break;
    default:
        val.order = __defaultPrecOrder;
        break;
    }

    return val;
}
