#pragma once

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
        /// translation units because of types defined in a function.
        VisibleNoLinkage,

        /// Internal linkage according to the Modules TS, but can be referred
        /// to from other translation units indirectly through functions and
        /// templates in the module interface.
        ModuleInternalLinkage,

        /// Module linkage, which indicates that the entity can be referred
        /// to from other translation units within the same module, and indirectly
        /// from arbitrary other translation units through functions and
        /// templates in the module interface.
        ModuleLinkage,

        /// External linkage, which indicates that the entity can
        /// be referred to from other translation units.
        ExternalLinkage
    };
} // namespace weasel
