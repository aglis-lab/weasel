#include <weasel/Package/FileAST.h>

FunctionHandle FileAST::getFunction(string_view funName, TypeHandle type, bool isStatic)
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

FunctionList FileAST::findFunctions(string_view funName, TypeHandle type, bool isStatic)
{
    FunctionList items;
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

StructTypeHandle FileAST::findStructType(string_view structName) const
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

void FileAST::addGlobalVariable(GlobalVariableHandle val)
{
    _globalVariables.push_back(val);
}

GlobalVariableList &FileAST::getGlobalVariables()
{
    return _globalVariables;
}

void FileAST::addUserType(StructTypeHandle type)
{
    _userTypes.push_back(type);
}

StructTypeList &FileAST::getUserTypes()
{
    return _userTypes;
}

void FileAST::addFunction(FunctionHandle fun)
{
    _functions.push_back(fun);
}

FunctionList &FileAST::getFunctions()
{
    return _functions;
}
