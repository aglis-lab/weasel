#pragma once

#include <weasel/AST/AST.h>

namespace weasel
{
    class Builder
    {
    public:
        Builder(/* args */) {}
        ~Builder() {}

    private:
        CompoundStatement _statement;
    };
} // namespace weasel
