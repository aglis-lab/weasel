#include <iostream>
#include "llvm/IR/Function.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Module.h"
#include "weasel/IR/Context.h"
#include "weasel/Symbol/Symbol.h"
#include "weasel/Config/Config.h"

weasel::Context::Context(llvm::LLVMContext *context, const std::string &moduleName, bool isParallel)
{
    _context = context;
    _module = new llvm::Module(moduleName, *_context);
    _mdBuilder = new llvm::MDBuilder(*_context);
    _builder = new llvm::IRBuilder<>(*_context);
}

llvm::Function *weasel::Context::codegen(weasel::Function *funAST)
{
    _currentFunction = funAST;

    auto funName = funAST->getIdentifier();
    auto isVararg = funAST->getFunctionType()->getIsVararg();
    auto funArgs = funAST->getArgs();
    auto retTy = funAST->getFunctionType()->getReturnType();
    auto parallelType = funAST->getParallelType();
    auto args = std::vector<llvm::Type *>();
    auto argsLength = funArgs.size() - (isVararg ? 1 : 0);

    // Set Arguments
    for (size_t i = 0; i < argsLength; i++)
    {
        auto arg = funArgs[i]->getArgumentType();

        args.push_back(arg);
    }

    auto linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
    auto *funTyLLVM = llvm::FunctionType::get(retTy, args, isVararg);
    auto *funLLVM = llvm::Function::Create(funTyLLVM, linkage, funName, *getModule());

    if (funAST->isInline())
    {
        funLLVM->addFnAttr(llvm::Attribute::AttrKind::InlineHint);
        funLLVM->addFnAttr(llvm::Attribute::AttrKind::AlwaysInline);
    }

    funLLVM->setDSOLocal(true);
    if (parallelType == ParallelType::ParallelKernel)
    {
        funLLVM->setCallingConv(llvm::CallingConv::SPIR_KERNEL);
    }
    else if (parallelType == ParallelType::ParallelFunction)
    {
        funLLVM->setCallingConv(llvm::CallingConv::SPIR_FUNC);
    }

    // if (parallelFun)
    // {
    //     funLLVM->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Local);
    //     funLLVM->addFnAttr(llvm::Attribute::AttrKind::NoUnwind);
    //     funLLVM->addFnAttr(llvm::Attribute::AttrKind::Convergent);
    //     funLLVM->addFnAttr(llvm::Attribute::AttrKind::NoRecurse);
    //     funLLVM->addFnAttr(llvm::Attribute::AttrKind::NoFree);
    // }

    // Add Function to symbol table
    {
        auto attr = new Attribute(funName, AttributeScope::ScopeGlobal, AttributeKind::SymbolFunction, funLLVM);
        SymbolTable::insert(funName, attr);
    }

    if (funAST->isDefine())
    {
        auto *entry = llvm::BasicBlock::Create(*getContext(), "", funLLVM);
        getBuilder()->SetInsertPoint(entry);

        // Enter to parameter scope
        SymbolTable::enterScope();

        auto idx = 0;
        for (auto &item : funLLVM->args())
        {
            auto argExpr = funArgs[idx++];
            auto *paramTy = argExpr->getArgumentType();
            auto argName = argExpr->getArgumentName();

            item.setName(argName);

            if ((paramTy->isPointerTy() || paramTy->isArrayTy()) && parallelType != ParallelType::None)
            {
                item.addAttr(llvm::Attribute::AttrKind::NoCapture);

                // TODO: Add Modifier const or final
                item.addAttr(llvm::Attribute::AttrKind::ReadOnly);
            }

            auto attrKind = AttributeKind::SymbolVariable;
            if (paramTy->isArrayTy())
            {
                attrKind = AttributeKind::SymbolArray;
            }
            else if (paramTy->isPointerTy())
            {
                attrKind = AttributeKind::SymbolPointer;
            }

            auto attr = new Attribute(argName, AttributeScope::ScopeParam, attrKind, &item);
            SymbolTable::insert(argName, attr);
        }

        // Create Block
        funAST->getBody()->codegen(this);
        if (funLLVM->getReturnType()->isVoidTy())
        {
            getBuilder()->CreateRetVoid();
        }

        // Exit from parameter scope
        {
            auto exit = SymbolTable::exitScope();
            if (!exit)
            {
                return nullptr;
            }
        }
    }

    return funLLVM;
}

llvm::Value *weasel::Context::codegen(StatementExpression *expr)
{
    // Enter to new statement
    {
        SymbolTable::enterScope();
    }

    for (auto &item : expr->getBody())
    {
        item->codegen(this);
    }

    // Exit from statement
    {
        SymbolTable::exitScope();
    }

    return nullptr;
}

llvm::Value *weasel::Context::codegen(CallExpression *expr)
{
    auto identifier = expr->getIdentifier();
    auto args = expr->getArguments();
    auto *fun = getModule()->getFunction(identifier);
    auto callConv = fun->getCallingConv();

    std::vector<llvm::Value *> argsV;
    for (size_t i = 0; i < args.size(); i++)
    {
        argsV.push_back(args[i]->codegen(this));
        if (!argsV.back())
        {
            return ErrorTable::addError(expr->getToken(), "Expected argument list index " + std::to_string(i));
        }
    }

    auto *call = getBuilder()->CreateCall(fun, argsV);
    if (fun->hasFnAttribute(llvm::Attribute::AttrKind::Convergent))
    {
        call->setCallingConv(llvm::CallingConv::SPIR_FUNC);
        call->setTailCall();
    }
    else
    {
        call->setCallingConv(llvm::CallingConv::C);
    }

    return call;
}

llvm::Value *weasel::Context::codegen(NumberLiteralExpression *expr) const
{
    return getBuilder()->getInt32(expr->getValue());
}

llvm::Value *weasel::Context::codegen(ArrayLiteralExpression *expr)
{
    auto items = expr->getItems();
    auto numItem = items.size();
    auto *valueTy = llvm::ArrayType::get(getBuilder()->getInt32Ty(), numItem);
    auto *valueNull = llvm::Constant::getNullValue(valueTy);
    auto valueArr = std::vector<llvm::Constant *>(numItem);
    auto i = 0;

    while (auto *c = valueNull->getAggregateElement(i))
    {
        if (auto *constVal = llvm::dyn_cast<llvm::Constant>(items[i]->codegen(this)))
        {
            valueArr[i] = constVal;
        }
        else
        {
            valueArr[i] = c;
        }

        i++;
    }

    auto init = llvm::ConstantArray::get(valueTy, valueArr);
    auto linkage = llvm::GlobalVariable::LinkageTypes::PrivateLinkage;
    auto *gv = new llvm::GlobalVariable(*getModule(), valueTy, true, linkage, init);
    auto dataLayout = llvm::DataLayout(getModule());
    auto alignNum = dataLayout.getPrefTypeAlignment(valueTy);

    gv->setAlignment(llvm::Align(std::max((unsigned int)16, alignNum)));
    gv->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Local);

    return getBuilder()->CreateLoad(gv->getType(), gv);
}

llvm::Value *weasel::Context::codegen(StringLiteralExpression *expr) const
{
    auto *str = getBuilder()->CreateGlobalString(expr->getValue());
    std::vector<llvm::Value *> idxList;
    idxList.push_back(getBuilder()->getInt64(0));
    idxList.push_back(getBuilder()->getInt64(0));

    return llvm::ConstantExpr::getGetElementPtr(str->getType()->getElementType(), str, idxList, true);
}

llvm::Value *weasel::Context::codegen() const
{
    return llvm::ConstantPointerNull::getNullValue(getBuilder()->getInt8PtrTy());
}

llvm::Value *weasel::Context::codegen(DeclarationExpression *expr)
{
    // Get Value Representation
    llvm::Value *value = nullptr;
    llvm::Type *declTy = expr->getType();

    auto exprValue = expr->getValue();
    if (exprValue != nullptr)
    {
        if ((value = exprValue->codegen(this)) != nullptr)
        {
            if (value->getType()->getTypeID() == llvm::Type::TypeID::VoidTyID)
            {
                return ErrorTable::addError(exprValue->getToken(), "Cannot assign void to a variable");
            }

            if (value->getValueID() == llvm::Value::ConstantPointerNullVal && declTy)
            {
                value = llvm::ConstantPointerNull::getNullValue(declTy);
            }

            if (declTy)
            {
                auto compareTy = compareType(declTy, value->getType());
                if (compareTy == CompareType::Different)
                {
                    return ErrorTable::addError(exprValue->getToken(), "Cannot assign, expression type is different");
                }

                if (compareTy == CompareType::Casting)
                {
                    value = castIntegerType(value, declTy);
                }
            }
            else
            {
                declTy = value->getType();
            }
        }
    }

    // Allocating Address for declaration
    auto varName = expr->getIdentifier();
    auto *alloc = getBuilder()->CreateAlloca(declTy, nullptr, varName);

    // Add Variable Declaration to symbol table
    {
        AttributeKind attrKind;
        if (declTy->isArrayTy())
        {
            attrKind = AttributeKind::SymbolArray;
        }
        else if (declTy->isPointerTy())
        {
            attrKind = AttributeKind::SymbolPointer;
        }
        else
        {
            attrKind = AttributeKind::SymbolVariable;
        }

        auto attr = new Attribute(varName, AttributeScope::ScopeLocal, attrKind, alloc);
        SymbolTable::insert(varName, attr);
    }

    if (value)
    {
        getBuilder()->CreateStore(value, alloc);
    }

    return alloc;
}

llvm::Value *weasel::Context::codegen(BinaryOperatorExpression *expr)
{
    auto token = expr->getOperator();
    auto *rhs = expr->getRHS()->codegen(this);
    auto *lhs = expr->getLHS()->codegen(this);
    // auto compareTy = compareType(lhs->getType(), rhs->getType());

    // if (compareTy == CompareType::Different)
    // {
    //     return logErrorV(std::string("type LHS != type RHS"));
    // }

    // if (compareTy == CompareType::Casting)
    // {
    //     castIntegerType(lhs, rhs);
    // }

    switch (token.getTokenKind())
    {
    case TokenKind::TokenOperatorStar:
        return getBuilder()->CreateMul(lhs, rhs, lhs->getName());
    case TokenKind::TokenOperatorSlash:
        return getBuilder()->CreateSDiv(lhs, rhs, lhs->getName());
    // case TokenKind::TokenPuncPercent: return llvm::BinaryOperator::
    case TokenKind::TokenOperatorPlus:
        return getBuilder()->CreateAdd(lhs, rhs, lhs->getName());
    case TokenKind::TokenOperatorMinus:
        return getBuilder()->CreateSub(lhs, rhs, lhs->getName());
    case TokenKind::TokenOperatorEqual:
    {
        auto *loadLhs = llvm::dyn_cast<llvm::LoadInst>(lhs);
        if (!loadLhs)
        {
            return ErrorTable::addError(expr->getLHS()->getToken(), "LHS not valid");
        }

        auto *allocLhs = loadLhs->getPointerOperand();
        getBuilder()->CreateStore(rhs, allocLhs);

        return getBuilder()->CreateLoad(allocLhs->getType(), allocLhs);
    }
    default:
        std::cout << "HELLO ERROR\n";
        return nullptr;
    }
}

llvm::Value *weasel::Context::codegen(ReturnExpression *expr)
{
    if (!expr->getValue())
    {
        return getBuilder()->CreateRetVoid();
    }

    auto *val = expr->getValue()->codegen(this);

    // Get Last Function from symbol table
    auto funAttr = SymbolTable::getLastFunction();
    if (!funAttr)
    {
        return ErrorTable::addError(expr->getToken(), "Return Statement cannot find last function from symbol table");
    }
    auto *fun = llvm::dyn_cast<llvm::Function>(funAttr->getValue());
    auto *returnTy = fun->getReturnType();
    auto compareTy = compareType(returnTy, val->getType());

    if (compareTy == CompareType::Different)
    {
        return ErrorTable::addError(expr->getToken(), "Return Type with value type is different");
    }

    if (compareTy == CompareType::Casting)
    {
        val = castIntegerType(val, returnTy);
    }

    return getBuilder()->CreateRet(val);
}

llvm::Value *weasel::Context::codegen(VariableExpression *expr) const
{
    // Get Allocator from Symbol Table
    auto varName = expr->getIdentifier();
    auto attr = SymbolTable::get(varName);
    if (!attr)
    {
        return ErrorTable::addError(expr->getToken(), "Variable " + varName + " Not declared");
    }

    auto *alloc = attr->getValue();
    if (expr->isAddressOf())
    {
        return alloc;
    }

    if (llvm::dyn_cast<llvm::Argument>(alloc))
    {
        return alloc;
    }

    return getBuilder()->CreateLoad(alloc->getType(), alloc, varName);
}

llvm::Value *weasel::Context::codegen(ArrayExpression *expr)
{
    // Get Allocator from Symbol Table
    auto varName = expr->getIdentifier();
    auto attr = SymbolTable::get(varName);
    auto *alloc = attr->getValue();
    auto indexValue = expr->getIndex()->codegen(this);
    auto longTy = getBuilder()->getInt64Ty();

    auto compare = compareType(indexValue->getType(), longTy);
    if (compare == CompareType::Different)
    {
        return ErrorTable::addError(expr->getToken(), "Expected integer value");
    }

    if (compare == CompareType::Casting)
    {
        indexValue = castIntegerType(indexValue, longTy);
    }

    std::vector<llvm::Value *> idxList;
    if (attr->isKind(AttributeKind::SymbolArray))
    {
        idxList.push_back(getBuilder()->getInt64(0));
    }
    idxList.push_back(indexValue);

    if (attr->isKind(AttributeKind::SymbolPointer))
    {
        if (llvm::dyn_cast<llvm::Instruction>(alloc))
        {
            alloc = getBuilder()->CreateLoad(alloc->getType(), alloc, "pointerLoad");
        }
    }

    auto *elemIndex = getBuilder()->CreateInBoundsGEP(alloc->getType(), alloc, idxList, "arrayElement");
    if (expr->isAddressOf())
    {
        return elemIndex;
    }

    auto *loadIns = getBuilder()->CreateLoad(elemIndex->getType(), elemIndex, varName);

    if (_currentFunction->getParallelType() != ParallelType::None)
    {
        auto *node = getTBAA(loadIns->getType());
        auto *mdTBAA = getMDBuilder()->createTBAAStructTagNode(node, node, 0);
        loadIns->setMetadata(llvm::LLVMContext::MD_tbaa, mdTBAA);
    }

    return loadIns;
}
