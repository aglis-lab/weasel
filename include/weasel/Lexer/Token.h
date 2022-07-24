#pragma once

#include <string>
#include "weasel/Basic/Enum.h"

#define __defaultPrecOrder 20

// Base
namespace weasel
{
    // Token Kind
    enum class TokenKind
    {
        // End Of File
        TokenEOF,

        // Identifier
        TokenIdentifier,

        // Keyword
        TokenKeyParallel,         // parallel // heterogeneous support
        TokenKeyKernel,           // kernel // heterogeneous kernel type
        TokenKeyInline,           // For Always inline attribute
        TokenKeyFun,              // fun
        TokenKeyExtern,           // extern
        TokenKeyStartDeclaration, // START DECLARATION
        TokenKeyLet,              // let
        TokenKeyFinal,            // final
        TokenKeyConst,            // const
        TokenKeyEndDeclaration,   // END DECLARATION
        TokenKeyReturn,           // return
        TokenKeyIf,               // if // Condition
        TokenKeyElse,             // else // Condition
        TokenKeyFor,              // for // Looping
        TokenKeyBreak,            // break // looping
        TokenKeyContinue,         // continue //looping
        TokenKeyAssert,           // assert // debugging

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
        TokenOperatorMinus,            // -
        TokenOperatorStar,             // *
        TokenOperatorSlash,            // /
        TokenOperatorPercent,          // %
        TokenOperatorCaret,            // ^
        TokenOperatorNot,              // !
        TokenOperatorAnd,              // &
        TokenOperatorOr,               // |
        TokenOperatorAndAnd,           // &&
        TokenOperatorOror,             // ||
        TokenOperatorShiftLeft,        // <<
        TokenOperatorShiftRight,       // >>
        TokenOperatorPlusEqual,        // +=
        TokenOperatorMinusEqual,       // -=
        TokenOperatorStarEqual,        // *=
        TokenOperatorSlashEqual,       // /=
        TokenOperatorPercentEqual,     // %=
        TokenOperatorCaretEqual,       // ^=
        TokenOperatorAndEqual,         // &=
        TokenOperatorOrEqual,          // |=
        TokenOperatorShiftLeftEqual,   // <<=
        TokenOperatorShiftRightEqual,  // >>=
        TokenOperatorEqual,            // =
        TokenOperatorStartComparation, // START COMPARATOR
        TokenOperatorLessThan,         // <
        TokenOperatorGreaterThen,      // >
        TokenOperatorEqualEqual,       // ==
        TokenOperatorNotEqual,         // !=
        TokenOperatorLessEqual,        // <=
        TokenOperatorGreaterEqual,     // >=
        TokenOperatorEndComparation,   // END COMPARATOR
        TokenOperatorEnd,              // END OPERATOR

        TokenPuncDot,       // .
        TokenPuncDotThree,  // ...
        TokenPuncComma,     // ,
        TokenPuncPound,     // #
        TokenPuncQuestion,  // ?
        TokenPuncSemicolon, // ; // optional

        // Punctuation Delimiter Type
        TokenDelimOpenCurlyBracket,   // {
        TokenDelimCloseCurlyBracket,  // }
        TokenDelimOpenSquareBracket,  // [
        TokenDelimCloseSquareBracket, // ]
        TokenDelimOpenParen,          // (
        TokenDelimCloseParen,         // )

        // Space Token
        TokenSpaceNewline, // '\n'

        // Wild card Token
        TokenUndefined,
    };

    // Qualifier for Variable
    enum class Qualifier
    {
        QualConst,
        QualRestrict,
        QualVolatile,
    };

    // Source Location
    struct SourceLocation
    {
        unsigned row = 1;
        unsigned col = 1;
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
    class Context;

    class Token
    {
    private:
        char *_startBuffer;
        char *_endBuffer;

        TokenKind _kind;
        SourceLocation _location;

    public:
        Token(TokenKind kind, SourceLocation location, char *startToken, char *endToken) : _startBuffer(startToken), _endBuffer(endToken), _kind(kind), _location(location) {}
        Token() : _kind(TokenKind::TokenUndefined) {}

        // Fast Checking //
        inline bool isDataType() { return _kind >= TokenKind::TokenTyVoid && _kind <= TokenKind::TokenTyDecimal; }
        inline bool isKeyDefinition() { return (_kind == TokenKind::TokenKeyLet || _kind == TokenKind::TokenKeyFinal || _kind == TokenKind::TokenKeyConst); }
        inline bool isLiteral() { return _kind >= TokenKind::TokenLitNil && _kind <= TokenKind::TokenLitString; }
        inline bool isOperator() { return _kind >= TokenKind::TokenOperatorStart && _kind <= TokenKind::TokenOperatorEnd; }
        inline bool isUndefined() const { return _kind == TokenKind::TokenUndefined; }
        inline bool isKind(TokenKind type) const { return type == _kind; }
        inline bool isNewline() const { return _kind == TokenKind::TokenSpaceNewline; }
        inline bool isOpenParen() const { return _kind == TokenKind::TokenDelimOpenParen; }
        inline bool isCloseParen() const { return _kind == TokenKind::TokenDelimCloseParen; }
        inline bool isOpenCurly() const { return _kind == TokenKind::TokenDelimOpenCurlyBracket; }
        inline bool isCloseCurly() const { return _kind == TokenKind::TokenDelimCloseCurlyBracket; }
        inline bool isSemiColon() const { return _kind == TokenKind::TokenPuncSemicolon; }

        // Operator Comparator //
        inline bool isComparison() { return _kind >= TokenKind::TokenOperatorStartComparation && _kind <= TokenKind::TokenOperatorEndComparation; }
        inline bool isComparisonLessThan() const { return _kind == TokenKind::TokenOperatorLessThan; }
        inline bool isComparisonGreaterThan() const { return _kind == TokenKind::TokenOperatorGreaterThen; }
        inline bool isComparisonEqualEqual() const { return _kind == TokenKind::TokenOperatorEqualEqual; }
        inline bool isComparisonNotEqual() const { return _kind == TokenKind::TokenOperatorNotEqual; }
        inline bool isComparisonLessEqual() const { return _kind == TokenKind::TokenOperatorLessEqual; }
        inline bool isComparisonGreaterEqual() const { return _kind == TokenKind::TokenOperatorGreaterEqual; }

        // Keyword //
        inline bool isKeyFor() const { return _kind == TokenKind::TokenKeyFor; }
        inline bool isKeyDeclaration() { return _kind >= TokenKind::TokenKeyStartDeclaration && _kind <= TokenKind::TokenKeyEndDeclaration; }
        inline bool isKeyBreak() const { return _kind == TokenKind::TokenKeyBreak; }
        inline bool isKeyContinue() const { return _kind == TokenKind::TokenKeyContinue; }

        // Condition //
        inline bool isKeyElse() const { return _kind == TokenKind::TokenKeyElse; }
        inline bool isKeyIf() const { return _kind == TokenKind::TokenKeyIf; }

        // Buffer //
        inline char *getStartBuffer() const { return _startBuffer; }
        inline char *getEndBuffer() const { return _endBuffer; }

        std::string getValue() const { return std::string(_startBuffer, _endBuffer); }

        SourceLocation getLocation() const { return _location; }
        TokenKind getTokenKind() const { return _kind; }
        int getTokenKindToInt() { return enumToInt(_kind); }

        Qualifier getQualifier() const;

        // TODO: Need to support Right to Left Associativity
        Precedence getPrecedence();

        inline static Token empty() { return Token(); }
    };

} // namespace weasel
