#include <iostream>
#include <llvm/IR/Function.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Module.h>
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

llvm::Type *weasel::Context::codegen(weasel::Type *type)
{
    if (type->isVoidType())
    {
        return getBuilder()->getVoidTy();
    }

    if (type->isIntegerType())
    {
        return getBuilder()->getIntNTy(type->getTypeWidth());
    }

    if (type->isFloatType())
    {
        return getBuilder()->getFloatTy();
    }

    if (type->isDoubleType())
    {
        return getBuilder()->getDoubleTy();
    }

    if (type->isArrayType())
    {
        auto containedType = type->getContainedType()->codegen(this);
        return llvm::ArrayType::get(containedType, type->getTypeWidth());
    }

    if (type->isPointerType())
    {
        auto containedType = type->getContainedType()->codegen(this);
        return llvm::PointerType::get(containedType, type->getTypeWidth());
    }

    return nullptr;
}

llvm::Function *weasel::Context::codegen(weasel::Function *funAST)
{
    _currentFunction = funAST;

    auto funName = funAST->getIdentifier();
    auto isVararg = funAST->getFunctionType()->getIsVararg();
    auto funArgs = funAST->getArgs();
    auto retTy = funAST->getFunctionType()->getReturnType();
    auto args = std::vector<llvm::Type *>();
    auto argsLength = funArgs.size() - (isVararg ? 1 : 0);

    // Set Arguments
    for (size_t i = 0; i < argsLength; i++)
    {
        auto arg = funArgs[i]->getArgumentType();
        auto argV = arg->codegen(this);

        args.push_back(argV);
    }

    auto linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
    auto funTyLLVM = llvm::FunctionType::get(retTy->codegen(this), args, isVararg);
    auto funLLVM = llvm::Function::Create(funTyLLVM, linkage, funName, *getModule());

    if (funAST->isInline())
    {
        funLLVM->addFnAttr(llvm::Attribute::AttrKind::InlineHint);
        funLLVM->addFnAttr(llvm::Attribute::AttrKind::AlwaysInline);
    }

    funLLVM->setDSOLocal(true);

    // Add Function to symbol table
    {
        auto attr = new Attribute(funName, AttributeScope::ScopeGlobal, AttributeKind::SymbolFunction, funLLVM);
        SymbolTable::insert(funName, attr);
    }

    if (funAST->isDefine())
    {
        auto entry = llvm::BasicBlock::Create(*getContext(), getDefaultLabel(), funLLVM);
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

            // if ((paramTy->isPointerTy() || paramTy->isArrayTy()) && parallelType != ParallelType::None)
            // {
            //     item.addAttr(llvm::Attribute::AttrKind::NoCapture);
            //     item.addAttr(llvm::Attribute::AttrKind::ReadOnly);
            // }

            auto attrKind = AttributeKind::SymbolVariable;
            if (paramTy->isArrayType())
            {
                attrKind = AttributeKind::SymbolArray;
            }
            else if (paramTy->isPointerType())
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

// TODO: Not Implemented Yet
llvm::Value *weasel::Context::codegen(StatementExpression *expr)
{
    // Enter to new statement
    {
        SymbolTable::enterScope();
    }

    for (auto &item : expr->getBody())
    {
        auto val = item->codegen(this);
    }

    // Exit from statement
    {
        SymbolTable::exitScope();
    }

    return nullptr;
}

llvm::Value *weasel::Context::codegen(IfStatementExpression *expr)
{
}

// TODO: Not Fully Implemented
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

    auto call = getBuilder()->CreateCall(fun, argsV);
    call->setCallingConv(llvm::CallingConv::C);

    return call;
}

llvm::Value *weasel::Context::codegen(NilLiteralExpression *expr) const
{
    return llvm::ConstantPointerNull::getNullValue(getBuilder()->getInt8PtrTy());
}

llvm::Value *weasel::Context::codegen(DeclarationExpression *expr)
{
    // Get Value Representation
    auto declType = expr->getType();
    auto valueExpr = expr->getValue();

    if (declType == nullptr && valueExpr != nullptr && valueExpr->getType() != nullptr)
    {
        declType = new Type(*valueExpr->getType());
        expr->setType(declType);
    }

    // Allocating Address for declaration
    auto varName = expr->getIdentifier();
    auto declTypeV = declType->codegen(this);
    auto alloc = getBuilder()->CreateAlloca(declTypeV, nullptr, varName);

    // Default Value
    if (valueExpr == nullptr)
    {
        // Default Value for integer
        if (declType->isIntegerType())
        {
            auto constantVal = llvm::ConstantInt::get(declTypeV, 0, declType->isSigned());
            getBuilder()->CreateStore(constantVal, alloc);
        }
    }
    else
    {
        auto valueType = valueExpr->getType();
        if (valueType->isVoidType())
        {
            return ErrorTable::addError(valueExpr->getToken(), "Cannot assign void to a variable");
        }

        if (declType != nullptr && declType->getTypeID() != valueType->getTypeID())
        {
            return ErrorTable::addError(valueExpr->getToken(), "Cannot assign to different type");
        }

        if (declType != nullptr && declType->getTypeWidth() != valueType->getTypeWidth())
        {
            return ErrorTable::addError(valueExpr->getToken(), "Cannot assign to different size type");
        }

        auto valueV = valueExpr->codegen(this);
        getBuilder()->CreateStore(valueV, alloc);
    }

    // Add Variable Declaration to symbol table
    {
        AttributeKind attrKind;
        if (declType->isArrayType())
        {
            attrKind = AttributeKind::SymbolArray;
        }
        else if (declType->isPointerType())
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

    // auto value = expr->getValue();
    // if (value)
    // {
    //     getBuilder()->CreateStore(value, alloc);
    // }

    return alloc;
}

// TODO: Need Type Check and conversion
llvm::Value *weasel::Context::codegen(BinaryOperatorExpression *expr)
{
    auto token = expr->getOperator();
    auto lhs = expr->getLHS();
    auto rhs = expr->getRHS();

    // Checking Type
    if (lhs->getType()->getTypeID() != rhs->getType()->getTypeID())
    {
        ErrorTable::addError(expr->getLHS()->getToken(), "Data type look different");

        return lhs->codegen(this);
    }

    auto lhsVal = lhs->codegen(this);
    auto rhsVal = rhs->codegen(this);

    if (token.isKind(TokenKind::TokenOperatorEqual))
    {
        auto loadLhs = llvm::dyn_cast<llvm::LoadInst>(lhsVal);
        if (!loadLhs)
        {
            return ErrorTable::addError(expr->getLHS()->getToken(), "LHS not valid");
        }

        auto allocLhs = loadLhs->getPointerOperand();
        getBuilder()->CreateStore(rhsVal, allocLhs);

        return getBuilder()->CreateLoad(allocLhs->getType(), allocLhs);
    }

    if (lhs->getType()->isFloatType() || lhs->getType()->isDoubleType())
    {
        switch (token.getTokenKind())
        {
        case TokenKind::TokenOperatorStar:
            return getBuilder()->CreateFMul(lhsVal, rhsVal, lhsVal->getName());
        case TokenKind::TokenOperatorSlash:
            return getBuilder()->CreateFDiv(lhsVal, rhsVal, lhsVal->getName());
        // case TokenKind::TokenPuncPercent: return llvm::BinaryOperator::
        case TokenKind::TokenOperatorPlus:
            return getBuilder()->CreateFAdd(lhsVal, rhsVal, lhsVal->getName());
        case TokenKind::TokenOperatorMinus:
            return getBuilder()->CreateFAdd(lhsVal, rhsVal, lhsVal->getName());
        default:
            ErrorTable::addError(expr->getLHS()->getToken(), "Not Yet Implemented Operator");
            return lhsVal;
        }
    }

    switch (token.getTokenKind())
    {
    case TokenKind::TokenOperatorStar:
        return getBuilder()->CreateMul(lhsVal, rhsVal, lhsVal->getName());
    case TokenKind::TokenOperatorSlash:
        return getBuilder()->CreateSDiv(lhsVal, rhsVal, lhsVal->getName());
    // case TokenKind::TokenPuncPercent: return llvm::BinaryOperator::
    case TokenKind::TokenOperatorPlus:
        return getBuilder()->CreateAdd(lhsVal, rhsVal, lhsVal->getName());
    case TokenKind::TokenOperatorMinus:
        return getBuilder()->CreateSub(lhsVal, rhsVal, lhsVal->getName());
    default:
        ErrorTable::addError(expr->getLHS()->getToken(), "Not Yet Implemented Operator");
        return lhsVal;
    }
}

// TODO: Check Function Return Type and Return Type
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
    if (attr == nullptr)
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

// TODO: String as array of byte
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

    return loadIns;
}
