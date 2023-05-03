#pragma once

#include <vector>

#include <weasel/AST/AST.h>

namespace weasel
{
    class Module
    {
    public:
        std::vector<GlobalVariable *> getGlobalVariables() const { return _globalVariables; }
        void addGlobalVariable(GlobalVariable *val) { _globalVariables.push_back(val); }

        std::vector<StructType *> getUserTypes() const { return _userTypes; }
        void addUserType(StructType *type) { _userTypes.push_back(type); }

        std::vector<Function *> getFunctions() const { return _functions; }
        void addFunction(Function *fun) { _functions.push_back(fun); }

    private:
        std::vector<GlobalVariable *> _globalVariables;
        std::vector<StructType *> _userTypes;
        std::vector<Function *> _functions;
    };
} // namespace weasel
