#pragma once

#include <weasel/AST/AST.h>

namespace weasel
{
    class Builder
    {
    public:
        Builder() {}
        ~Builder() {}

        void setCompoundStatemen(CompoundStatement *statement);

    private:
        CompoundStatement *_statement;
    };
} // namespace weasel
