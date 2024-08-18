#pragma once

#include <vector>

#include <weasel/AST/AST.h>

using namespace std;

namespace weasel
{
    class Module;
    using ModuleHandle = shared_ptr<Module>;

    class Module
    {
    private:
        vector<GlobalVariableHandle> _globalVariables;
        vector<StructTypeHandle> _userTypes;
        vector<FunctionHandle> _functions;

    public:
        explicit Module() = default;

        void addGlobalVariable(GlobalVariableHandle val)
        {
            _globalVariables.push_back(val);
        }
        vector<GlobalVariableHandle> &getGlobalVariables() { return _globalVariables; }

        void addUserType(StructTypeHandle type) { _userTypes.push_back(type); }
        vector<StructTypeHandle> &getUserTypes() { return _userTypes; }

        void addFunction(FunctionHandle fun) { _functions.push_back(fun); }
        vector<FunctionHandle> &getFunctions() { return _functions; }
        FunctionHandle getFunction(string_view funName, TypeHandle type = nullptr, bool isStatic = true)
        {
            for (auto item : getFunctions())
            {
                if (item->getIdentifier() == funName)
                {
                    if (!item->getImplType() && !type)
                    {
                        return item;
                    }

                    if (item->getImplType() && item->getImplType()->isEqual(type) && item->getFunctionType()->getIstatic() == isStatic)
                    {
                        return item;
                    }
                }
            }

            return nullptr;
        }
        vector<FunctionHandle> findFunctions(string_view funName, TypeHandle type = nullptr, bool isStatic = true)
        {
            vector<FunctionHandle> items;
            for (auto item : getFunctions())
            {
                if (item->getIdentifier() == funName)
                {
                    if (!item->getImplType() && !type)
                    {
                        items.push_back(item);
                    }
                    else if (item->getImplType() && item->getImplType()->isEqual(type) && item->getFunctionType()->getIstatic() == isStatic)
                    {
                        items.push_back(item);
                    }
                }
            }

            return items;
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
    };
} // namespace weasel
