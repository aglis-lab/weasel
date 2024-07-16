#pragma once

#include <vector>

#include "weasel/AST/AST.h"

namespace weasel
{
    class Module
    {
    public:
        explicit Module() = default;

        std::vector<GlobalVariableHandle> &getGlobalVariables() { return _globalVariables; }
        void addGlobalVariable(GlobalVariableHandle val) { _globalVariables.push_back(val); }

        std::vector<StructTypeHandle> &getUserTypes() { return _userTypes; }
        void addUserType(StructTypeHandle type) { _userTypes.push_back(type); }

        void addFunction(FunctionHandle fun) { _functions.push_back(fun); }
        std::vector<FunctionHandle> &getFunctions() { return _functions; }
        FunctionHandle findFunction(string_view funName)
        {
            for (auto item : getFunctions())
            {
                if (item->getIdentifier() == funName)
                {
                    return item;
                }
            }

            return nullptr;
        }

        StructTypeHandle findStructType(string_view structName) const
        {
            for (auto item : _userTypes)
            {
                if (item->getIdentifier() == structName)
                {
                    return item;
                }
            }

            return nullptr;
        }

    private:
        std::vector<GlobalVariableHandle> _globalVariables;
        std::vector<StructTypeHandle> _userTypes;
        std::vector<FunctionHandle> _functions;
    };
} // namespace weasel
