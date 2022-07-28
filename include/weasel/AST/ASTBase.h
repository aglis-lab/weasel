#pragma once

// Include Standard Library
#include <string>
#include <vector>

#include "weasel/Type/Type.h"
#include "weasel/Lexer/Token.h"
#include "weasel/Type/Type.h"
#include "weasel/Basic/Cast.h"

namespace llvm
{
    class Value;
} // namespace llvm

namespace weasel
{
    // Linkage //
    enum class Linkage
    {
        /// No linkage, which means that the entity is unique and
        /// can only be referred to from within its scope.
        NoLinkage = 0,

        /// Internal linkage, which indicates that the entity can
        /// be referred to from within the translation unit (but not other
        /// translation units).
        InternalLinkage,

        /// External linkage within a unique namespace.
        ///
        /// From the language perspective, these entities have external
        /// linkage. However, since they reside in an anonymous namespace,
        /// their names are unique to this translation unit, which is
        /// equivalent to having internal linkage from the code-generation
        /// point of view.
        UniqueExternalLinkage,

        /// No linkage according to the standard, but is visible from other
        /// translation units because of types defined in a inline function.
        VisibleNoLinkage,

        /// Internal linkage according to the Modules TS, but can be referred
        /// to from other translation units indirectly through inline functions and
        /// templates in the module interface.
        ModuleInternalLinkage,

        /// Module linkage, which indicates that the entity can be referred
        /// to from other translation units within the same module, and indirectly
        /// from arbitrary other translation units through inline functions and
        /// templates in the module interface.
        ModuleLinkage,

        /// External linkage, which indicates that the entity can
        /// be referred to from other translation units.
        ExternalLinkage
    };
} // namespace weasel

// Expression Base Type
namespace weasel
{
    // For debugging purpose
    class ASTDebug
    {
    protected:
        int defaultShift = 2;

    protected:
        void printDebug(const std::string &val, int shift);

    public:
        virtual void debug(int shift) = 0;
    };

    // Expression
    class Expression : public ASTDebug
    {
    protected:
        Token _token; // Token each expression
        Type *_type;

    public:
        Expression() = default;
        Expression(Token token) : _token(token) {}
        Expression(Token token, Type *type) : _token(token), _type(type) {}

        inline Token getToken() const { return _token; }
        inline Type *getType() const { return _type; }
        inline void setType(Type *type) { _type = type; }
        inline bool isNoType() const { return _type == nullptr; }

        virtual llvm::Value *codegen(Context *context) = 0;

    public:
        bool isCompoundExpression();
    };

    // Global Value
    class GlobalObject : public Expression
    {
    protected:
        Linkage _linkage;
        std::string _identifier;

    public:
        GlobalObject(Token token, const std::string &identifier, Type *type) : Expression(token, type), _identifier(identifier) {}

        inline std::string getIdentifier() const { return _identifier; }
    };

    // Literal Expression
    class LiteralExpression : public Expression
    {
    public:
        LiteralExpression(Token token, Type *type) : Expression(token, type) {}
    };
} // namespace weasel
