#pragma once

#include <string>

#include "weasel/Basic/Enum.h"
#include "weasel/Basic/Location.h"

constexpr uint __defaultPrecOrder = 20;

// Base
namespace weasel
{
    // Token Kind
    enum class TokenKind
    {
        // Wild card Token
        TokenUnknown,

        // End Of File
        TokenEOF,

        // Identifier
        TokenIdentifier,

        // Keyword
        TokenKeyImpl,             // impl // user type or block struct traits
        TokenKeyThis,             // referencing impl struct
        TokenKeyParallel,         // parallel // heterogeneous support
        TokenKeyKernel,           // kernel // heterogeneous kernel type
        TokenKeyStruct,           // struct
        TokenKeyInline,           // For Always attribute
        TokenKeyFun,              // function
        TokenKeyExtern,           // extern
        TokenKeyReturn,           // return
        TokenKeyIf,               // if // Condition
        TokenKeyElse,             // else // Condition
        TokenKeyFor,              // for // Looping
        TokenKeyBreak,            // break // looping
        TokenKeyContinue,         // continue //looping
        TokenKeyStartDeclaration, // START DECLARATION
        TokenKeyLet,              // let
        TokenKeyFinal,            // final
        TokenKeyConst,            // const
        TokenKeyEndDeclaration,   // END DECLARATION
        TokenKeyAssert,           // assert // debugging
        TokenKeyDefer,            // defer
        TokenKeyReadOnly,         // readonly
        TokenKeyWriteOnly,        // writeonly

        // Data Literal -> Value of data type
        TokenLitNil,
        TokenLitBool,
        TokenLitInteger,
        TokenLitFloat,
        TokenLitDouble,
        TokenLitChar,
        TokenLitString,

        /// Data Types -> literally name of the data type
        TokenTyVoid,    // Void No Return Value
        TokenTyRune,    // 4 byte Unicode // Character 'utf-32'
        TokenTyString,  // Character 'utf-8' with 'utf-32' support
        TokenTyByte,    // 1 byte // Integer Character 'utf-8'
        TokenTySbyte,   // 1 byte // Integer
        TokenTyShort,   // 2 byte // Integer
        TokenTyUshort,  // 2 byte // Integer
        TokenTyInt,     // 4 byte // Integer
        TokenTyUint,    // 4 byte // Integer
        TokenTyLong,    // 8 byte // Integer
        TokenTyUlong,   // 8 byte // Integer
        TokenTyInt128,  // 16 byte // Integer
        TokenTyUnt128,  // 16 byte // Integer // NOT USED
        TokenTyBool,    // 1 byte // Boolean
        TokenTyFloat,   // 4 byte // Floating Point
        TokenTyDouble,  // 8 byte // Floating Point
        TokenTyDecimal, // 16 byte // Floating Point

        // Punctuation
        TokenOperatorStart,            // START OPERATOR
        TokenOperatorPlus,             // +
        TokenOperatorNegative,         // -
        TokenOperatorStar,             // *
        TokenOperatorSlash,            // /
        TokenOperatorPercent,          // %
        TokenOperatorCaret,            // ^
        TokenOperatorNot,              // !
        TokenOperatorNegation,         // ~
        TokenOperatorAnd,              // &
        TokenOperatorOr,               // |
        TokenOperatorLogicalStart,     // START LOGICAL
        TokenOperatorAndAnd,           // &&
        TokenOperatorOror,             // ||
        TokenOperatorLogicalEnd,       // END LOGICAL
        TokenOperatorShiftLeft,        // <<
        TokenOperatorShiftRight,       // >>
        TokenOperatorEqualStart,       // START ASSIGNMENT
        TokenOperatorEqual,            // =
        TokenOperatorPlusEqual,        // +=
        TokenOperatorNegativeEqual,    // -=
        TokenOperatorStarEqual,        // *=
        TokenOperatorSlashEqual,       // /=
        TokenOperatorPercentEqual,     // %=
        TokenOperatorCaretEqual,       // ^=
        TokenOperatorShiftLeftEqual,   // <<=
        TokenOperatorShiftRightEqual,  // >>=
        TokenOperatorNegationEqual,    // ~=
        TokenOperatorAndEqual,         // &=
        TokenOperatorOrEqual,          // |=
        TokenOperatorEqualEnd,         // END ASSIGNMENT
        TokenOperatorStartComparation, // START COMPARATOR
        TokenOperatorLessThan,         // <
        TokenOperatorGreaterThen,      // >
        TokenOperatorEqualEqual,       // ==
        TokenOperatorNotEqual,         // !=
        TokenOperatorLessEqual,        // <=
        TokenOperatorGreaterEqual,     // >=
        TokenOperatorEndComparation,   // END COMPARATOR
        TokenOperatorCasting,          // as
        TokenOperatorEnd,              // END OPERATOR

        TokenPuncDot,       // .
        TokenPuncDotThree,  // ...
        TokenPuncComma,     // ,
        TokenPuncPound,     // #
        TokenPuncQuestion,  // ?
        TokenPuncColon,     // :
        TokenPuncSemicolon, // ;

        // Punctuation Delimiter Type
        TokenDelimOpenCurlyBracket,   // {
        TokenDelimCloseCurlyBracket,  // }
        TokenDelimOpenSquareBracket,  // [
        TokenDelimCloseSquareBracket, // ]
        TokenDelimOpenParen,          // (
        TokenDelimCloseParen,         // )

        // Space Token
        TokenSpaceNewline, // '\n'
    };

    // Qualifier for Variable
    enum class Qualifier
    {
        QualConst,
        QualRestrict,
        QualVolatile,
    };

    // Associativity
    enum class Associative
    {
        LeftToRight,
        RightToLeft,
    };

    // Precendence
    struct Precedence
    {
        Associative associative;
        unsigned order;
    };

} // namespace weasel

// Token Class
namespace weasel
{
    class Token
    {
    public:
        // Keyword Checking //
        bool isKind(TokenKind type) const { return type == _kind; }
        bool isKeyFunction() const { return _kind == TokenKind::TokenKeyFun; }
        bool isKeyStruct() const { return _kind == TokenKind::TokenKeyStruct; }
        bool isIdentifier() const { return _kind == TokenKind::TokenIdentifier; }
        bool isKeyDefer() const { return _kind == TokenKind::TokenKeyDefer; }
        bool isKeyImpl() const { return _kind == TokenKind::TokenKeyImpl; }
        bool isKeyThis() const { return _kind == TokenKind::TokenKeyThis; }
        bool isKeyExtern() const { return _kind == TokenKind::TokenKeyExtern; }

        // Variable //
        bool isDataType() const { return _kind >= TokenKind::TokenTyVoid && _kind <= TokenKind::TokenTyDecimal; }
        bool isKeyDefinition() const { return (_kind == TokenKind::TokenKeyLet || _kind == TokenKind::TokenKeyFinal || _kind == TokenKind::TokenKeyConst); }
        bool isLiteral() const { return _kind >= TokenKind::TokenLitNil && _kind <= TokenKind::TokenLitString; }

        // Operator //
        bool isOperator() const { return _kind >= TokenKind::TokenOperatorStart && _kind <= TokenKind::TokenOperatorEnd; }
        bool isUnknown() const { return _kind == TokenKind::TokenUnknown; }
        bool isNewline() const { return _kind == TokenKind::TokenSpaceNewline; }
        bool isOperatorCast() const { return _kind == TokenKind::TokenOperatorCasting; }

        bool isOperatorEqual() const { return _kind == TokenKind::TokenOperatorEqual; }
        bool isOperatorAssignment() const
        {
            return _kind > TokenKind::TokenOperatorEqualStart &&
                   _kind < TokenKind::TokenOperatorEqualEnd;
        }

        bool isOperatorAnd() const { return _kind == TokenKind::TokenOperatorAnd; }
        bool isOperatorStar() const { return _kind == TokenKind::TokenOperatorStar; }
        bool isOperatorNegative() const { return _kind == TokenKind::TokenOperatorNegative; }
        bool isOperatorPlus() const { return _kind == TokenKind::TokenOperatorPlus; }
        bool isOperatorNot() const { return _kind == TokenKind::TokenOperatorNot; }
        bool isOperatorNegation() const { return _kind == TokenKind::TokenOperatorNegation; }
        bool isOperatorUnary() const
        {
            return isOperatorAnd() ||
                   isOperatorStar() ||
                   isOperatorNegative() ||
                   isOperatorPlus() ||
                   isOperatorNegation() ||
                   isOperatorNot();
        }

        bool isOperatorAndAnd() const { return _kind == TokenKind::TokenOperatorAndAnd; }
        bool isOperatorOrOr() const { return _kind == TokenKind::TokenOperatorOror; }
        bool isOperatorLogical() const
        {
            return _kind > TokenKind::TokenOperatorLogicalStart &&
                   _kind < TokenKind::TokenOperatorLogicalEnd;
        }

        // Delimiter //
        bool isOpenParen() const { return _kind == TokenKind::TokenDelimOpenParen; }
        bool isCloseParen() const { return _kind == TokenKind::TokenDelimCloseParen; }
        bool isOpenCurly() const { return _kind == TokenKind::TokenDelimOpenCurlyBracket; }
        bool isCloseCurly() const { return _kind == TokenKind::TokenDelimCloseCurlyBracket; }
        bool isSemiColon() const { return _kind == TokenKind::TokenPuncSemicolon; }
        bool isOpenSquare() const { return _kind == TokenKind::TokenDelimOpenSquareBracket; }
        bool isCloseSquare() const { return _kind == TokenKind::TokenDelimCloseSquareBracket; }

        // Punctuation //
        bool isComma() const { return _kind == TokenKind::TokenPuncComma; }
        bool isColon() const { return _kind == TokenKind::TokenPuncColon; }
        bool isDot() const { return _kind == TokenKind::TokenPuncDot; }

        // Operator Comparator //
        bool isComparison() { return _kind >= TokenKind::TokenOperatorStartComparation && _kind <= TokenKind::TokenOperatorEndComparation; }
        bool isComparisonLessThan() const { return _kind == TokenKind::TokenOperatorLessThan; }
        bool isComparisonGreaterThan() const { return _kind == TokenKind::TokenOperatorGreaterThen; }
        bool isComparisonEqualEqual() const { return _kind == TokenKind::TokenOperatorEqualEqual; }
        bool isComparisonNotEqual() const { return _kind == TokenKind::TokenOperatorNotEqual; }
        bool isComparisonLessEqual() const { return _kind == TokenKind::TokenOperatorLessEqual; }
        bool isComparisonGreaterEqual() const { return _kind == TokenKind::TokenOperatorGreaterEqual; }

        // Keyword //
        bool isKeyParallel() const { return _kind == TokenKind::TokenKeyParallel; }
        bool isKeyFor() const { return _kind == TokenKind::TokenKeyFor; }
        bool isKeyDeclaration() const { return _kind >= TokenKind::TokenKeyStartDeclaration && _kind <= TokenKind::TokenKeyEndDeclaration; }
        bool isKeyBreak() const { return _kind == TokenKind::TokenKeyBreak; }
        bool isKeyContinue() const { return _kind == TokenKind::TokenKeyContinue; }
        bool isKeyReturn() const { return _kind == TokenKind::TokenKeyReturn; }

        // Condition //
        bool isKeyElse() const { return _kind == TokenKind::TokenKeyElse; }
        bool isKeyIf() const { return _kind == TokenKind::TokenKeyIf; }

        // Check If Last Buffer //
        bool isEnd() const { return _kind == TokenKind::TokenEOF; }

        // Buffer //
        char *getStartBuffer() const { return _startBuffer; }
        char *getEndBuffer() const { return _endBuffer; }

        string getValue() const { return string(getStartBuffer(), getEndBuffer()); }
        string getEscapeValue() const
        {
            string val = "";
            auto temp = getStartBuffer();
            while (temp != getEndBuffer())
            {
                switch (*temp)
                {
                case '\\':
                    val += "\\\\";
                    break;
                case '\n':
                    val += "\\n";
                    break;
                case '\r':
                    val += "\\r";
                    break;
                case '\t':
                    val += "\\t";
                    break;
                default:
                    val += *temp;
                }

                temp++;
            }

            return val;
        }

        SourceLocation getLocation() const { return _location; }
        TokenKind getTokenKind() const { return _kind; }
        int getTokenKindToInt() const { return enumToInt(_kind); }

        Qualifier getQualifier() const;

        // TODO: Need to support Right to Left Associativity
        Precedence getPrecedence();

        static Token empty() { return Token(); }

    public:
        static Token create();
        static Token create(TokenKind kind, SourceLocation location, char *startToken, char *endToken);

    protected:
        Token() : _kind(TokenKind::TokenUnknown) {}
        Token(TokenKind kind, SourceLocation location, char *startToken, char *endToken) : _startBuffer(startToken), _endBuffer(endToken), _kind(kind), _location(location) {}

    private:
        char *_startBuffer;
        char *_endBuffer;

        TokenKind _kind;
        SourceLocation _location;
    };

} // namespace weasel
