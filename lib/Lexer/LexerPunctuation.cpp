#include "weasel/Lexer/Lexer.h"

Token Lexer::getPunctuation()
{
    auto *lastBuffer = _currentBuffer - 1;

    switch (*lastBuffer)
    {
    case '+':
    {
        if (*(lastBuffer + 1) == '=')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorPlusEqual, lastBuffer, lastBuffer + 2);
        }

        return this->createToken(TokenKind::TokenOperatorPlus, lastBuffer, lastBuffer + 1);
    }
    case '-':
    {
        if (*(lastBuffer + 1) == '=')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorNegativeEqual, lastBuffer, lastBuffer + 2);
        }

        return this->createToken(TokenKind::TokenOperatorNegative, lastBuffer, lastBuffer + 1);
    }
    case '*':
    {
        if (*(lastBuffer + 1) == '=')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorStarEqual, lastBuffer, lastBuffer + 2);
        }

        return this->createToken(TokenKind::TokenOperatorStar, lastBuffer, lastBuffer + 1);
    }
    case '/':
    {
        if (*(lastBuffer + 1) == '=')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorSlashEqual, lastBuffer, lastBuffer + 2);
        }

        return this->createToken(TokenKind::TokenOperatorSlash, lastBuffer, lastBuffer + 1);
    }
    case '%':
    {
        if (*(lastBuffer + 1) == '=')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorPercentEqual, lastBuffer, lastBuffer + 2);
        }

        return this->createToken(TokenKind::TokenOperatorPercent, lastBuffer, lastBuffer + 1);
    }
    case '^':
    {
        if (*(lastBuffer + 1) == '=')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorCaretEqual, lastBuffer, lastBuffer + 2);
        }

        return this->createToken(TokenKind::TokenOperatorCaret, lastBuffer, lastBuffer + 1);
    }
    case '!':
    {
        if (*(lastBuffer + 1) == '=')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorNotEqual, lastBuffer, lastBuffer + 2);
        }

        return this->createToken(TokenKind::TokenOperatorNot, lastBuffer, lastBuffer + 1);
    }
    case '~':
    {
        if (*(lastBuffer + 1) == '=')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorNegationEqual, lastBuffer, lastBuffer + 2);
        }

        return this->createToken(TokenKind::TokenOperatorNegation, lastBuffer, lastBuffer + 1);
    }
    case '&':
    {
        if (*(lastBuffer + 1) == '=')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorAndEqual, lastBuffer, lastBuffer + 2);
        }

        if (*(lastBuffer + 1) == '&')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorAndAnd, lastBuffer, lastBuffer + 2);
        }

        return this->createToken(TokenKind::TokenOperatorAnd, lastBuffer, lastBuffer + 1);
    }
    case '|':
    {
        if (*(lastBuffer + 1) == '=')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorOrEqual, lastBuffer, lastBuffer + 2);
        }
        if (*(lastBuffer + 1) == '|')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorOror, lastBuffer, lastBuffer + 2);
        }

        return this->createToken(TokenKind::TokenOperatorOr, lastBuffer, lastBuffer + 1);
    }
    case '<':
    {
        if (*(lastBuffer + 1) == '=')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorLessEqual, lastBuffer, lastBuffer + 2);
        }

        if (*(lastBuffer + 1) == '<')
        {
            if (*(lastBuffer + 2) == '=')
            {
                getNextBuffer(2);
                return this->createToken(TokenKind::TokenOperatorShiftLeftEqual, lastBuffer, lastBuffer + 3);
            }

            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorShiftLeft, lastBuffer, lastBuffer + 2);
        }

        return this->createToken(TokenKind::TokenOperatorLessThan, lastBuffer, lastBuffer + 1);
    }
    case '>':
    {
        if (*(lastBuffer + 1) == '=')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorGreaterEqual, lastBuffer, lastBuffer + 2);
        }

        if (*(lastBuffer + 1) == '>')
        {
            if (*(lastBuffer + 2) == '=')
            {
                getNextBuffer(2);
                return this->createToken(TokenKind::TokenOperatorShiftRightEqual, lastBuffer, lastBuffer + 3);
            }

            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorShiftRight, lastBuffer, lastBuffer + 2);
        }

        return this->createToken(TokenKind::TokenOperatorGreaterThen, lastBuffer, lastBuffer + 1);
    }
    case '.':
    {
        if (*(lastBuffer + 1) == '.')
        {
            if (*(lastBuffer + 2) == '.')
            {
                getNextBuffer(2);
                return this->createToken(TokenKind::TokenPuncDotThree, lastBuffer, lastBuffer + 3);
            }

            return Token::empty();
        }

        return this->createToken(TokenKind::TokenPuncDot, lastBuffer, lastBuffer + 1);
    }
    case '#':
    {
        return this->createToken(TokenKind::TokenPuncPound, lastBuffer, lastBuffer + 1);
    }
    case '?':
    {
        return this->createToken(TokenKind::TokenPuncQuestion, lastBuffer, lastBuffer + 1);
    }
    case '=':
    {
        if (*(lastBuffer + 1) == '=')
        {
            getNextBuffer();
            return this->createToken(TokenKind::TokenOperatorEqualEqual, lastBuffer, lastBuffer + 2);
        }

        return this->createToken(TokenKind::TokenOperatorEqual, lastBuffer, lastBuffer + 1);
    }
    case ',':
    {
        return this->createToken(TokenKind::TokenPuncComma, lastBuffer, lastBuffer + 1);
    }
    case '(':
    {
        return this->createToken(TokenKind::TokenDelimOpenParen, lastBuffer, lastBuffer + 1);
    }
    case ')':
    {
        return this->createToken(TokenKind::TokenDelimCloseParen, lastBuffer, lastBuffer + 1);
    }
    case '[':
    {
        return this->createToken(TokenKind::TokenDelimOpenSquareBracket, lastBuffer, lastBuffer + 1);
    }
    case ']':
    {
        return this->createToken(TokenKind::TokenDelimCloseSquareBracket, lastBuffer, lastBuffer + 1);
    }
    case '{':
    {
        return this->createToken(TokenKind::TokenDelimOpenCurlyBracket, lastBuffer, lastBuffer + 1);
    }
    case '}':
    {
        return this->createToken(TokenKind::TokenDelimCloseCurlyBracket, lastBuffer, lastBuffer + 1);
    }
    case ';':
    {
        return this->createToken(TokenKind::TokenPuncSemicolon, lastBuffer, lastBuffer + 1);
    }
    case ':':
    {
        return this->createToken(TokenKind::TokenPuncColon, lastBuffer, lastBuffer + 1);
    }

    default:
        return Token::empty();
    }
}
