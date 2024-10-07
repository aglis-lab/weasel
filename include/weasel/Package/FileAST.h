#pragma once

#include <vector>

#include <llvm/IR/Module.h>

#include <weasel/AST/AST.h>

using namespace std;

namespace weasel
{
    class FileAST;
    class Package;

    using FileASTHandle = shared_ptr<FileAST>;
    using GlobalVariableList = vector<GlobalVariableHandle>;
    using StructTypeList = vector<StructTypeHandle>;
    using FunctionList = vector<FunctionHandle>;

    class FileAST
    {
    private:
        string _filename;
        Package *_package;
        GlobalVariableList _globalVariables;
        StructTypeList _userTypes;
        FunctionList _functions;

        llvm::Module *_llvmModule;

    public:
        explicit FileAST(Package *package) : _package(package)
        {
            // _llvmModule = llvm::Module()
        }

        void addGlobalVariable(GlobalVariableHandle val);
        GlobalVariableList &getGlobalVariables();

        void addUserType(StructTypeHandle type);
        StructTypeList &getUserTypes();

        void addFunction(FunctionHandle fun);
        FunctionList &getFunctions();
        FunctionHandle getFunction(string_view funName, TypeHandle type = nullptr, bool isStatic = true);

        FunctionList findFunctions(string_view funName, TypeHandle type = nullptr, bool isStatic = true);
        StructTypeHandle findStructType(string_view structName) const;
    };
} // namespace weasel
