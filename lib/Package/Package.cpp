#include <weasel/Package/Package.h>

using namespace weasel;

Package::Package()
{
    _fileHeader = make_shared<FileAST>(this);
}

FileASTHandle Package::createModule()
{
    _fileASTList.push_back(make_shared<FileAST>(this));
    return _fileASTList.back();
}

void Package::loadPackageHeader()
{
    // Load all ast from modules
    for (auto &mod : getFileASTList())
    {
        // Global Variables
        for (auto &item : mod->getGlobalVariables())
        {
            _fileHeader->getGlobalVariables().push_back(item);
        }

        // User Types
        for (auto &item : mod->getUserTypes())
        {
            _fileHeader->getUserTypes().push_back(item);
        }

        // Functions
        for (auto &item : mod->getFunctions())
        {
            _fileHeader->getFunctions().push_back(item);
        }
    }
}

vector<StructTypeHandle> &Package::getUserTypes()
{
    return _fileHeader->getUserTypes();
}

vector<FunctionHandle> &Package::getFunctions()
{
    return _fileHeader->getFunctions();
}

vector<GlobalVariableHandle> &Package::getGlobalVariables()
{
    return _fileHeader->getGlobalVariables();
}

StructTypeHandle Package::findStructType(string_view structName)
{
    for (auto &mod : getFileASTList())
    {
        auto val = mod->findStructType(structName);
        if (val)
        {
            return val;
        }
    }

    return nullptr;
}

vector<FunctionHandle> Package::findFunctions(string_view funName, TypeHandle type, bool isStatic)
{
    vector<FunctionHandle> funs;
    for (auto &mod : getFileASTList())
    {
        auto val = mod->findFunctions(funName, type, isStatic);
        if (val.empty())
        {
            for (auto &item : val)
            {
                funs.push_back(item);
            }
        }
    }

    return funs;
}

FunctionHandle Package::getFunction(string_view funName, TypeHandle type, bool isStatic)
{
    for (auto &mod : getFileASTList())
    {
        auto val = mod->getFunction(funName, type, isStatic);
        if (val)
        {
            return val;
        }
    }

    return nullptr;
}

void Package::setPackageName(string name)
{
    _packageName = name;
}

string Package::getPackageName() const
{
    return _packageName;
}
