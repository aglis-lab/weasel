#include <iostream>
#include <llvm/IR/Function.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Module.h>
#include "weasel/IR/Context.h"
#include "weasel/Symbol/Symbol.h"
#include "weasel/Config/Config.h"

weasel::Context::Context(llvm::LLVMContext *context, const std::string &moduleName)
{
    _context = context;
    _module = new llvm::Module(moduleName, *_context);
    _mdBuilder = new llvm::MDBuilder(*_context);
    _builder = new llvm::IRBuilder<>(*_context);
}

llvm::Value *weasel::Context::codegen(weasel::Function *funAST)
{
    _currentFunction = funAST;

    auto funName = funAST->getIdentifier();
    auto funType = funAST->getType();
    auto isVararg = funType->isSpread();
    auto funArgs = funType->getContainedTypes();
    auto argsLength = funArgs.size() - (isVararg ? 1 : 0);
    auto args = std::vector<llvm::Type *>(argsLength);
    for (int index = 0; index < argsLength; index++)
    {
        args[index] = funArgs[index]->codegen(this);
    }

    auto linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
    auto funTyLLVM = llvm::FunctionType::get(funType->codegen(this), args, isVararg);
    auto funLLVM = llvm::Function::Create(funTyLLVM, linkage, funName, *getModule());
    if (funAST->isInline())
    {
        funLLVM->addFnAttr(llvm::Attribute::AttrKind::InlineHint);
        funLLVM->addFnAttr(llvm::Attribute::AttrKind::AlwaysInline);
    }

    funLLVM->setDSOLocal(true);

    // Add Function to symbol table
    {
        auto attr = new Attribute(funName, AttributeScope::ScopeGlobal, AttributeKind::SymbolFunction, funLLVM, funType);
        SymbolTable::insert(funName, attr);
    }

    if (funAST->isDefine())
    {
        auto entry = llvm::BasicBlock::Create(*getContext(), "entry", funLLVM);
        getBuilder()->SetInsertPoint(entry);

        // Enter to parameter scope
        SymbolTable::enterScope();

        auto idx = 0;
        for (auto &item : funLLVM->args())
        {
            auto argExpr = funArgs[idx++];
            auto paramTy = argExpr;
            auto argName = argExpr->getIdentifier();

            item.setName(argName);

            auto attrKind = AttributeKind::SymbolVariable;
            if (paramTy->isArrayType())
            {
                attrKind = AttributeKind::SymbolArray;
            }
            else if (paramTy->isPointerType())
            {
                attrKind = AttributeKind::SymbolPointer;
            }

            auto attr = new Attribute(argName, AttributeScope::ScopeParam, attrKind, &item, paramTy);
            SymbolTable::insert(argName, attr);
        }

        // Create Block
        funAST->getBody()->codegen(this);
        if (funLLVM->getReturnType()->isVoidTy() && llvm::dyn_cast<llvm::ReturnInst>(&getBuilder()->GetInsertBlock()->back()) == nullptr)
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

llvm::Value *weasel::Context::codegen(MethodCallExpression *expr)
{
    auto identifier = expr->getIdentifier();
    auto args = expr->getArguments();
    auto fun = getModule()->getFunction(identifier);

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
    auto valueType = expr->getType();

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

        if (!valueType->isEqual(declType))
        {
            return ErrorTable::addError(valueExpr->getToken(), "Cannot assign to different type");
        }

        auto valueV = valueExpr->codegen(this);
        getBuilder()->CreateStore(valueV, alloc);
    }

    // Add Variable Declaration to symbol table
    {
        AttributeKind attrKind = AttributeKind::SymbolVariable;
        if (declType->isArrayType())
        {
            attrKind = AttributeKind::SymbolArray;
        }
        else if (declType->isPointerType())
        {
            attrKind = AttributeKind::SymbolPointer;
        }

        auto attr = new Attribute(varName, AttributeScope::ScopeLocal, attrKind, alloc, declType);
        SymbolTable::insert(varName, attr);
    }

    return alloc;
}

// TODO: Need Type Check and conversion
llvm::Value *weasel::Context::codegen(BinaryOperatorExpression *expr)
{
    auto opToken = expr->getOperator();
    auto lhs = expr->getLHS();
    auto rhs = expr->getRHS();
    auto lhsType = lhs->getType();
    auto rhsType = rhs->getType();
    auto exprType = expr->getType();

    // Checking Type
    if (!lhsType->isEqual(rhsType))
    {
        ErrorTable::addError(expr->getLHS()->getToken(), "Data type look different");

        return lhs->codegen(this);
    }

    auto lhsVal = lhs->codegen(this);
    auto rhsVal = rhs->codegen(this);
    auto isFloat = lhsType->isFloatType() || rhsType->isDoubleType();
    auto isSigned = exprType->isSigned();

    if (opToken.isComparison())
    {
        switch (opToken.getTokenKind())
        {
        case TokenKind::TokenOperatorLessThan:
        {
            if (isFloat)
            {
                return getBuilder()->CreateFCmpOLT(lhsVal, rhsVal);
            }

            if (isSigned)
            {
                return getBuilder()->CreateICmpSLT(lhsVal, rhsVal);
            }

            return getBuilder()->CreateICmpULT(lhsVal, rhsVal);
        }
        case TokenKind::TokenOperatorGreaterThen:
        {
            if (isFloat)
            {
                return getBuilder()->CreateFCmpOGT(lhsVal, rhsVal);
            }

            if (isSigned)
            {
                return getBuilder()->CreateICmpSGT(lhsVal, rhsVal);
            }

            return getBuilder()->CreateICmpUGT(lhsVal, rhsVal);
        }
        case TokenKind::TokenOperatorEqualEqual:
        {
            if (isFloat)
            {
                return getBuilder()->CreateFCmpOEQ(lhsVal, rhsVal);
            }

            return getBuilder()->CreateICmpEQ(lhsVal, rhsVal);
        }
        case TokenKind::TokenOperatorNotEqual:
        {
            if (isFloat)
            {
                return getBuilder()->CreateFCmpONE(lhsVal, rhsVal);
            }

            return getBuilder()->CreateICmpNE(lhsVal, rhsVal);
        }
        case TokenKind::TokenOperatorLessEqual:
        {

            if (isFloat)
            {
                return getBuilder()->CreateFCmpOLE(lhsVal, rhsVal);
            }

            if (isSigned)
            {
                return getBuilder()->CreateICmpSLE(lhsVal, rhsVal);
            }

            return getBuilder()->CreateICmpULE(lhsVal, rhsVal);
        }
        case TokenKind::TokenOperatorGreaterEqual:
        {
            if (isFloat)
            {
                return getBuilder()->CreateFCmpOGE(lhsVal, rhsVal);
            }

            if (isSigned)
            {
                return getBuilder()->CreateICmpSGE(lhsVal, rhsVal);
            }

            return getBuilder()->CreateICmpUGE(lhsVal, rhsVal);
        }
        default:
        {
            return nullptr;
        }
        }
    }

    if (opToken.isKind(TokenKind::TokenOperatorEqual))
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

    if (isFloat)
    {
        switch (opToken.getTokenKind())
        {
        case TokenKind::TokenOperatorStar:
            return getBuilder()->CreateFMul(lhsVal, rhsVal, lhsVal->getName());
        case TokenKind::TokenOperatorSlash:
            return getBuilder()->CreateFDiv(lhsVal, rhsVal, lhsVal->getName());
        case TokenKind::TokenOperatorPercent:
            return getBuilder()->CreateFRem(lhsVal, rhsVal);
        case TokenKind::TokenOperatorPlus:
            return getBuilder()->CreateFAdd(lhsVal, rhsVal, lhsVal->getName());
        case TokenKind::TokenOperatorMinus:
            return getBuilder()->CreateFAdd(lhsVal, rhsVal, lhsVal->getName());
        default:
            ErrorTable::addError(expr->getLHS()->getToken(), "Not Yet Implemented Operator");
            return lhsVal;
        }
    }

    switch (opToken.getTokenKind())
    {
    case TokenKind::TokenOperatorStar:
    {
        if (isSigned)
        {
            return getBuilder()->CreateNSWMul(lhsVal, rhsVal, lhsVal->getName());
        }
        return getBuilder()->CreateNUWMul(lhsVal, rhsVal, lhsVal->getName());
    }
    case TokenKind::TokenOperatorSlash:
    {
        if (isSigned)
        {
            return getBuilder()->CreateSDiv(lhsVal, rhsVal, lhsVal->getName());
        }

        return getBuilder()->CreateUDiv(lhsVal, rhsVal, lhsVal->getName());
    }
    case TokenKind::TokenOperatorPercent:
    {
        if (isSigned)
        {
            return getBuilder()->CreateSRem(lhsVal, rhsVal);
        }
        return getBuilder()->CreateURem(lhsVal, rhsVal);
    }
    case TokenKind::TokenOperatorPlus:
    {
        if (isSigned)
        {
            return getBuilder()->CreateNSWAdd(lhsVal, rhsVal, lhsVal->getName());
        }
        return getBuilder()->CreateNUWAdd(lhsVal, rhsVal, lhsVal->getName());
    }
    case TokenKind::TokenOperatorMinus:
    {
        if (isSigned)
        {
            return getBuilder()->CreateNSWSub(lhsVal, rhsVal, lhsVal->getName());
        }
        return getBuilder()->CreateNUWSub(lhsVal, rhsVal, lhsVal->getName());
    }
    default:
        ErrorTable::addError(expr->getLHS()->getToken(), "Not Yet Implemented Operator");
        return lhsVal;
    }
}

llvm::Value *weasel::Context::codegen(BreakExpression *expr)
{
    if (!isBreakBlockExist())
    {
        return ErrorTable::addError(expr->getToken(), "No looping found");
    }

    if (expr->getValue() == nullptr)
    {
        return getBuilder()->CreateBr(getBreakBlock());
    }

    auto condExpr = expr->getValue();
    auto condType = condExpr->getType();
    if (!condType->isBooleanType())
    {
        return ErrorTable::addError(expr->getToken(), "Break Condition should be boolean");
    }

    auto newBlock = llvm::BasicBlock::Create(*getContext(), "", getBuilder()->GetInsertBlock()->getParent());
    auto breakBlock = getBreakBlock();
    auto condVal = condExpr->codegen(this);
    auto brIns = getBuilder()->CreateCondBr(condVal, breakBlock, newBlock);

    // Create New Insert Point
    getBuilder()->SetInsertPoint(newBlock);

    return brIns;
}

llvm::Value *weasel::Context::codegen(ContinueExpression *expr)
{
    if (!isContinueBlockExist())
    {
        return ErrorTable::addError(expr->getToken(), "No looping found");
    }

    if (expr->getValue() == nullptr)
    {
        return getBuilder()->CreateBr(getContinueBlock());
    }

    auto condExpr = expr->getValue();
    auto condType = condExpr->getType();
    if (!condType->isBooleanType())
    {
        return ErrorTable::addError(expr->getToken(), "Continue Condition should be boolean");
    }

    auto newBlock = llvm::BasicBlock::Create(*getContext(), "", getBuilder()->GetInsertBlock()->getParent());
    auto continueBlock = getContinueBlock();
    auto condVal = condExpr->codegen(this);
    auto brIns = getBuilder()->CreateCondBr(condVal, continueBlock, newBlock);

    // // Create New Insert Point
    getBuilder()->SetInsertPoint(newBlock);

    return brIns;
}

llvm::Value *weasel::Context::codegen(ReturnExpression *expr)
{
    if (expr->getValue() == nullptr)
    {
        return getBuilder()->CreateRetVoid();
    }

    // Get Last Function from symbol table
    auto funAttr = SymbolTable::getLastFunction();
    auto val = expr->getValue()->codegen(this);
    auto fun = llvm::dyn_cast<llvm::Function>(funAttr->getValue());
    auto returnTy = fun->getReturnType();
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

llvm::Value *weasel::Context::codegen(VariableExpression *expr)
{
    // Get Allocator from Symbol Table
    auto varName = expr->getIdentifier();
    auto attr = SymbolTable::get(varName);
    if (attr == nullptr)
    {
        return ErrorTable::addError(expr->getToken(), "Variable " + varName + " Not declared");
    }

    auto alloc = attr->getValue();
    if (expr->isAddressOf())
    {
        return alloc;
    }

    if (llvm::dyn_cast<llvm::Argument>(alloc))
    {
        return alloc;
    }

    auto type = attr->getType()->codegen(this);
    return getBuilder()->CreateLoad(type, alloc, varName);
}

// TODO: String as array of byte
llvm::Value *weasel::Context::codegen(ArrayExpression *expr)
{
    // Get Allocator from Symbol Table
    auto varName = expr->getIdentifier();
    auto attr = SymbolTable::get(varName);
    auto alloc = attr->getValue();
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
