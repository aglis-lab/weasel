#pragma once

#include <vector>

#include <llvm/IR/LLVMContext.h>

#include <weasel/Package/FileAST.h>
#include <weasel/AST/AST.h>

using namespace std;

namespace weasel
{
    class Package;

    using PackageHandle = shared_ptr<Package>;
    using PackageList = vector<PackageHandle>;
    using FileASTList = vector<FileASTHandle>;

    class Package
    {
    private:
        FileASTList _fileASTList;
        FileASTHandle _fileHeader;
        PackageHandle _child;
        PackageHandle _parent;

        string _packageName;

        llvm::LLVMContext _llvmContext;

    public:
        explicit Package();

        FileASTHandle createModule();

        FileASTList &getFileASTList() { return _fileASTList; }

        StructTypeHandle findStructType(string_view structName);
        vector<FunctionHandle> findFunctions(string_view funName, TypeHandle type = nullptr, bool isStatic = true);
        FunctionHandle getFunction(string_view funName, TypeHandle type = nullptr, bool isStatic = true);

        // TODO: Using Multiple Thread
        void loadPackageHeader();
        vector<StructTypeHandle> &getUserTypes();
        vector<FunctionHandle> &getFunctions();
        vector<GlobalVariableHandle> &getGlobalVariables();

        void setPackageName(string name);
        string getPackageName() const;
    };
} // namespace weasel
