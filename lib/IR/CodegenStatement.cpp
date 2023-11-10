#include <weasel/IR/Codegen.h>
#include <weasel/Symbol/Symbol.h>

weasel::WeaselCodegen::WeaselCodegen(llvm::LLVMContext *context, const std::string &moduleName)
{
    _context = context;
    _module = new llvm::Module(moduleName, *_context);
    _mdBuilder = new llvm::MDBuilder(*_context);
    _builder = new llvm::IRBuilder<>(*_context);
}

llvm::Value *weasel::WeaselCodegen::castInteger(llvm::Value *val, llvm::Type *type, bool isSign)
{
    if (isSign)
    {
        return getBuilder()->CreateSExtOrTrunc(val, type);
    }

    return getBuilder()->CreateZExtOrTrunc(val, type);
}

llvm::Value *weasel::WeaselCodegen::codegen(weasel::Function *funAST)
{
    LOG(INFO) << "Codegen Function " << funAST->getIdentifier();

    auto funName = funAST->getIdentifier();
    auto funType = funAST->getType();
    auto isVararg = funType->isSpread();
    auto funArgs = funAST->getArguments();
    auto argsLength = (int)funArgs.size() - (isVararg ? 1 : 0);
    auto args = std::vector<llvm::Type *>(argsLength);
    for (int index = 0; index < argsLength; index++)
    {
        args[index] = funArgs[index]->getType()->codegen(this);
    }

    auto linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
    auto returnType = funType->codegen(this);
    auto funTyLLVM = llvm::FunctionType::get(returnType, args, isVararg);
    auto funLLVM = llvm::Function::Create(funTyLLVM, linkage, funName, getModule());
    if (funAST->isInline())
    {
        funLLVM->addFnAttr(llvm::Attribute::AttrKind::InlineHint);
        funLLVM->addFnAttr(llvm::Attribute::AttrKind::AlwaysInline);
    }

    funLLVM->setDSOLocal(true);
    if (funAST->isDefine())
    {
        auto entryBlock = llvm::BasicBlock::Create(*getContext(), "entry", funLLVM);
        getBuilder()->SetInsertPoint(entryBlock);

        // Enter to new scope
        enterScope();

        LOG(INFO) << "Codegen Function Arguments\n";

        // Allocate Arguments
        struct AllocateArgument
        {
            llvm::AllocaInst *alloca;
            llvm::Argument *arg;
        };

        std::vector<AllocateArgument> argAllocs;
        auto idx = 0;
        for (auto &item : funLLVM->args())
        {
            auto argExpr = funArgs[idx++];
            auto argName = argExpr->getArgumentName();

            item.setName(argName);

            // Allocate Argument
            auto alloc = getBuilder()->CreateAlloca(item.getType());
            argAllocs.push_back(AllocateArgument{alloc, &item});

            // Add Attribute
            addAttribute(ContextAttribute::get(argName, alloc, AttributeKind::Parameter));
        }

        // Create Block
        LOG(INFO) << "Codegen Function Body\n";

        // TODO: Make it more better
        // Traverse Alloca Expression to make sure every alloca is on the start
        traverseAllocaExpression(funAST->getBody());

        // Store argument value
        for (auto item : argAllocs)
        {
            getBuilder()->CreateStore(item.arg, item.alloca);
        }

        // Codegen Function Body
        funAST->getBody()->codegen(this);

        // Create Return Block and Value
        _returnBlock = llvm::BasicBlock::Create(*getContext());
        if (!funAST->getType()->isVoidType())
        {
            _returnValue = getBuilder()->CreateAlloca(funTyLLVM->getReturnType());
        }

        // Create Default Return Block
        if (llvm::dyn_cast<llvm::BranchInst>(&getBuilder()->GetInsertBlock()->back()) == nullptr)
        {
            getBuilder()->CreateBr(_returnBlock);
        }

        // Evaluate Return
        funLLVM->getBasicBlockList().push_back(_returnBlock);
        getBuilder()->SetInsertPoint(_returnBlock);

        if (funAST->getType()->isVoidType())
        {
            getBuilder()->CreateRetVoid();
        }
        else
        {
            auto loadReturnValue = getBuilder()->CreateLoad(funTyLLVM->getReturnType(), _returnValue);
            getBuilder()->CreateRet(loadReturnValue);
        }

        // Release Return Block and Value
        _returnValue = nullptr;
        _returnBlock = nullptr;

        // Exit fromscope
        exitScope();
    }

    return funLLVM;
}

llvm::Value *weasel::WeaselCodegen::codegen(CallExpression *expr)
{
    LOG(INFO) << "Codegen Call Function\n";

    auto identifier = expr->getFunction()->getIdentifier();
    auto args = expr->getArguments();
    auto fun = getModule()->getFunction(identifier);

    std::vector<llvm::Value *> argsV;
    for (size_t i = 0; i < args.size(); i++)
    {
        args[i]->setAccess(AccessID::Load);
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

llvm::Value *weasel::WeaselCodegen::codegen(BreakExpression *expr)
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
    if (!condType->isBoolType())
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

llvm::Value *weasel::WeaselCodegen::codegen(ContinueExpression *expr)
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
    if (!condType->isBoolType())
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

llvm::Value *weasel::WeaselCodegen::codegen(ReturnExpression *expr)
{
    LOG(INFO) << "Codegen Return Function\n";

    if (!expr->getType()->isVoidType())
    {
        auto val = expr->getValue()->codegen(this);
        if (expr->getType()->isIntegerType())
        {
            auto returnTyV = getBuilder()->GetInsertBlock()->getParent()->getReturnType();

            if (expr->getType()->isSigned())
            {
                val = getBuilder()->CreateSExtOrTrunc(val, returnTyV);
            }
            else
            {
                val = getBuilder()->CreateZExtOrTrunc(val, returnTyV);
            }
        }

        getBuilder()->CreateStore(val, _returnValue);
    }

    return getBuilder()->CreateBr(_returnBlock);
}

llvm::Value *weasel::WeaselCodegen::codegen(VariableExpression *expr)
{
    LOG(INFO) << "Codegen Variable";

    // Get Allocator from Symbol Table
    auto varName = expr->getIdentifier();
    auto type = expr->getType();
    auto attr = findAttribute(varName);
    if (attr.isEmpty())
    {
        return ErrorTable::addError(expr->getToken(), "Variable " + varName + " Not declared");
    }

    auto alloc = attr.getValue();
    if (llvm::dyn_cast<llvm::Argument>(alloc))
    {
        return alloc;
    }

    if (type->isStructType())
    {
        return getBuilder()->CreateBitCast(alloc, getBuilder()->getInt8PtrTy());
    }

    if (type->isArrayType())
    {
        return getBuilder()->CreateInBoundsGEP(type->codegen(this), alloc, {getBuilder()->getInt64(0), getBuilder()->getInt64(0)});
    }

    if (expr->isAccessAllocation())
    {
        return alloc;
    }

    return getBuilder()->CreateLoad(type->codegen(this), alloc);
}

// TODO: String as array of byte
llvm::Value *weasel::WeaselCodegen::codegen(ArrayExpression *expr)
{
    LOG(INFO) << "Codege Array Expression";

    // Get Index
    auto indexExpr = expr->getIndex();
    auto indexV = indexExpr->codegen(this);

    assert(indexV);
    assert(indexExpr);

    // Get Allocator from Symbol Table
    auto varName = expr->getIdentifier();
    auto attr = findAttribute(varName);

    assert(!attr.isEmpty() && "Cannot find variable");

    auto alloc = attr.getValue();

    assert(alloc);

    // Casting to integer 64
    indexV = getBuilder()->CreateSExtOrTrunc(indexV, getBuilder()->getInt64Ty());

    std::vector<llvm::Value *> idxList = {indexV};

    auto typeV = alloc->getType()->getPointerElementType();
    if (typeV->isPointerTy())
    {
        alloc = getBuilder()->CreateLoad(typeV, alloc);
        typeV = typeV->getPointerElementType();
    }
    else
    {
        idxList.insert(idxList.begin(), getBuilder()->getInt64(0));
    }

    auto elemIndex = getBuilder()->CreateInBoundsGEP(typeV, alloc, idxList);
    if (expr->isAccessAllocation())
    {
        return elemIndex;
    }

    return getBuilder()->CreateLoad(typeV, elemIndex);
}

llvm::Value *weasel::WeaselCodegen::codegen(FieldExpression *expr)
{
    auto parent = dynamic_cast<VariableExpression *>(expr->getParentField());
    auto type = dynamic_cast<StructType *>(parent->getType());
    assert(type != nullptr);

    auto typeV = type->codegen(this);
    auto attr = findAttribute(parent->getIdentifier());
    auto field = expr->getField();
    auto idx = type->findTypeName(field);

    auto range = getBuilder()->getInt32(0);
    auto idxVal = getBuilder()->getInt32(idx);
    auto inbound = getBuilder()->CreateInBoundsGEP(typeV, attr.getValue(), {range, idxVal});

    return getBuilder()->CreateLoad(inbound->getType()->getPointerElementType(), inbound);
}

llvm::Value *weasel::WeaselCodegen::codegen(StructExpression *expr)
{
    if (expr->getFields().empty())
    {
        return getBuilder()->getInt8(0);
    }

    auto type = dynamic_cast<StructType *>(expr->getType());
    auto isConstant = expr->getIsPreferConstant() && type->isPreferConstant();
    auto fields = expr->getFields();
    if (isConstant)
    {
        for (auto item : fields)
        {
            if (!dynamic_cast<LiteralExpression *>(item->getExpression()))
            {
                isConstant = false;
                break;
            }
        }
    }

    auto typeFields = type->getContainedTypes();
    auto fieldSize = (int)typeFields.size();
    if (isConstant)
    {
        std::vector<llvm::Constant *> arr(fieldSize, nullptr);
        for (auto &item : fields)
        {
            auto idx = type->findTypeName(item->getIdentifier());
            if (idx == -1)
            {
                continue;
            }

            arr[idx] = llvm::dyn_cast<llvm::Constant>(item->getExpression()->codegen(this));
        }

        for (int i = 0; i < fieldSize; i++)
        {
            if (arr[i] != nullptr)
            {
                continue;
            }

            auto typeField = typeFields[i];
            auto typeFieldV = typeField->codegen(this);
            if (typeField->isPrimitiveType())
            {
                if (typeField->isFloatType() || typeField->isDoubleType())
                {
                    arr[i] = llvm::ConstantFP::get(typeFieldV, 0);
                }
                else
                {
                    arr[i] = llvm::ConstantInt::get(typeFieldV, 0);
                }
            }
            else
            {
                arr[i] = llvm::ConstantStruct::get(llvm::dyn_cast<llvm::StructType>(typeFieldV), {});
            }
        }

        auto typeV = llvm::dyn_cast<llvm::StructType>(type->codegen(this));
        auto constantV = llvm::ConstantStruct::get(typeV, arr);
        auto val = new llvm::GlobalVariable(*getModule(), typeV, true, llvm::GlobalValue::LinkageTypes::PrivateLinkage, constantV);

        val->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);

        return val;
    }

    auto typeV = type->codegen(this);
    auto alloc = getBuilder()->CreateAlloca(typeV);

    getBuilder()->CreateMemSet(alloc, getBuilder()->getInt8(0), type->getTypeWidthByte(), llvm::MaybeAlign(4));

    for (auto item : fields)
    {
        auto idx = type->findTypeName(item->getIdentifier());
        if (idx == -1)
        {
            continue;
        }

        auto idxStruct = getBuilder()->getInt32(0);
        auto idxVal = getBuilder()->getInt32(idx);
        auto inbound = getBuilder()->CreateInBoundsGEP(typeV, alloc, {idxStruct, idxVal});
        auto val = item->getExpression()->codegen(this);

        getBuilder()->CreateStore(val, inbound);
    }

    return alloc;
}

llvm::Value *weasel::WeaselCodegen::codegen(DeclarationStatement *expr)
{
    LOG(INFO) << "Codegen Declaration Statement";

    // Get Value Representation
    auto declType = expr->getType();
    auto valueExpr = expr->getValue();

    // Allocating Address for declaration
    auto varName = expr->getIdentifier();
    auto declTypeV = declType->codegen(this);

    // Set Default Value if no value expression
    if (valueExpr == nullptr)
    {
        llvm::Constant *constantVal = nullptr;

        // Default Value for integer
        if (declType->isIntegerType())
        {
            constantVal = llvm::ConstantInt::get(declTypeV, 0, declType->isSigned());
        }

        // Default Value for Float
        if (declType->isFloatType())
        {
            constantVal = llvm::ConstantFP::get(declTypeV, 0);
        }

        // Store Default Value
        auto alloc = this->getAllocaMap(expr);
        if (constantVal != nullptr)
        {
            this->getBuilder()->CreateStore(constantVal, alloc);
        }

        // Add Variable Declaration to symbol table
        addAttribute(ContextAttribute::get(varName, alloc, AttributeKind::Variable));

        return nullptr;
    }

    // TODO: Change this to Analysis type checking
    auto valueType = valueExpr->getType();
    if (valueType->isVoidType())
    {
        return ErrorTable::addError(valueExpr->getToken(), "Cannot assign void to a variable");
    }

    // Check if type is different
    // TODO: Change this to Analysis type checking
    if (!valueType->isEqual(declType))
    {
        return ErrorTable::addError(valueExpr->getToken(), "Cannot assign to different type");
    }

    // Check if StructExpression
    // TODO: Change this to Analysis type checking
    {
        auto temp = dynamic_cast<StructExpression *>(valueExpr);
        if (temp != nullptr)
        {
            temp->setPreferConstant(true);
        }
    }

    // Codegen Value Expression
    // TODO: Change this to Analysis type checking
    auto valueV = valueExpr->codegen(this);
    if (valueV == nullptr)
    {
        return ErrorTable::addError(valueExpr->getToken(), "Cannot codegen value expression");
    }

    // TODO: LLVM Declare Struct Metadata
    // call void @llvm.dbg.declare(metadata %struct.Person* %3, metadata !20, metadata !DIExpression()), !dbg !28
    if (declType->isStructType())
    {
        auto widthVal = declType->getTypeWidthByte();
        auto alloc = valueV;

        if (llvm::dyn_cast<llvm::GlobalVariable>(valueV))
        {
            alloc = this->getBuilder()->CreateAlloca(declTypeV, nullptr);
            this->getBuilder()->CreateMemCpy(alloc, llvm::MaybeAlign(4), valueV, llvm::MaybeAlign(4), widthVal);
        }
        else if (llvm::dyn_cast<llvm::ConstantInt>(valueV))
        {
            alloc = this->getBuilder()->CreateAlloca(declTypeV, nullptr);
            this->getBuilder()->CreateMemSet(alloc, valueV, widthVal, llvm::MaybeAlign(0));
        }

        // Add Variable Declaration to symbol table
        addAttribute(ContextAttribute::get(varName, alloc, AttributeKind::Variable));

        return nullptr;
    }

    // TODO: Add metadata for auto type casting
    if (declType->isPrimitiveType() && declType->getTypeWidth() != valueType->getTypeWidth())
    {
        valueV = this->getBuilder()->CreateSExtOrTrunc(valueV, declTypeV);
    }

    // auto alloc = this->getBuilder()->CreateAlloca(declTypeV, nullptr);
    auto alloc = this->getAllocaMap(expr);

    // Check if Array Literal
    if (dynamic_cast<ArrayLiteralExpression *>(valueExpr))
    {
        auto arrayPtr = this->getBuilder()->CreateBitCast(alloc, getBuilder()->getInt8PtrTy());
        auto align = declType->getContainedType()->getTypeWidthByte();
        auto size = align * valueType->getTypeWidth();

        this->getBuilder()->CreateMemCpy(arrayPtr, llvm::MaybeAlign(align), valueV, llvm::MaybeAlign(align), getBuilder()->getInt64(size));
    }
    else
    {
        this->getBuilder()->CreateStore(valueV, alloc);
    }

    // Add Variable Declaration to symbol table
    addAttribute(ContextAttribute::get(varName, alloc, AttributeKind::Variable));

    return nullptr;
}

llvm::Value *weasel::WeaselCodegen::codegen(CompoundStatement *expr)
{
    LOG(INFO) << "Codegen Compound Statement";

    // Enter to new statement
    enterScope();

    for (auto &item : expr->getBody())
    {
        item->setAccess(AccessID::Allocation);
        item->codegen(this);
    }

    // Exit from statement
    exitScope();

    return nullptr;
}

llvm::Value *weasel::WeaselCodegen::codegen(ConditionStatement *expr)
{
    auto conditions = expr->getConditions();
    auto statements = expr->getStatements();
    auto count = (int)conditions.size();
    auto parentFun = this->getBuilder()->GetInsertBlock()->getParent();
    auto endBlock = llvm::BasicBlock::Create(*getContext());

    for (int i = 0; i < count; i++)
    {
        auto condition = conditions[i];
        auto statement = statements[i];
        auto conditionType = condition->getType();
        if (!conditionType->isBoolType())
        {
            return ErrorTable::addError(condition->getToken(), "Expected Boolean Type");
        }

        auto bodyBlock = llvm::BasicBlock::Create(*getContext(), "", parentFun);
        auto nextBlock = llvm::BasicBlock::Create(*getContext());

        // Create Condition Branch
        this->getBuilder()->CreateCondBr(condition->codegen(this), bodyBlock, nextBlock);

        // Set Insert Point
        this->getBuilder()->SetInsertPoint(bodyBlock);

        // Driver Body
        statement->codegen(this);

        // Jump to Next Block
        if (!this->getBuilder()->GetInsertBlock()->back().isTerminator())
        {
            this->getBuilder()->CreateBr(endBlock);
        }

        // Add Next Block to Fuction
        parentFun->getBasicBlockList().push_back(nextBlock);

        // Set Insert point
        this->getBuilder()->SetInsertPoint(nextBlock);
    }

    if (expr->isElseExist())
    {
        auto statement = statements.back();
        auto elseBlock = llvm::BasicBlock::Create(*getContext(), "", parentFun);

        this->getBuilder()->CreateBr(elseBlock);
        this->getBuilder()->SetInsertPoint(elseBlock);

        statement->codegen(this);
    }

    // Jump to Next Block
    if (!this->getBuilder()->GetInsertBlock()->back().isTerminator())
    {
        this->getBuilder()->CreateBr(endBlock);
    }

    // Add End Block to Fuction
    parentFun->getBasicBlockList().push_back(endBlock);

    // Set Insert point
    this->getBuilder()->SetInsertPoint(endBlock);

    return nullptr;
}

llvm::Value *weasel::WeaselCodegen::codegen(LoopingStatement *expr)
{
    LOG(INFO) << "Codegen For Loop Statement";

    auto isInfinity = expr->isInfinityCondition();
    auto isSingleCondition = expr->isSingleCondition();
    auto currentBlock = this->getBuilder()->GetInsertBlock();
    auto bodyBlock = llvm::BasicBlock::Create(*getContext());
    auto endBlock = llvm::BasicBlock::Create(*getContext());
    auto conditionBlock = llvm::BasicBlock::Create(*getContext());
    auto countBlock = llvm::BasicBlock::Create(*getContext());
    auto parentFun = currentBlock->getParent();
    auto conditions = expr->getConditions();

    // Make Sure every variable or expression have LHS Meta Data
    for (auto item : conditions)
    {
        if (item != nullptr)
        {
            item->setAccess(AccessID::Allocation);
        }
    }

    // Enter to new statement
    enterScope();

    // Add Last Block to Loop Blocks
    addbreakBlock(endBlock);
    addContinueBlock(conditionBlock);

    // Initial //
    if (!isInfinity && !isSingleCondition)
    {
        auto initialExpr = conditions[0];
        initialExpr->codegen(this);
    }

    // Condition //
    // Jump to Conditional
    this->getBuilder()->CreateBr(conditionBlock);

    // Set Insert point to Conditional Block
    parentFun->getBasicBlockList().push_back(conditionBlock);
    this->getBuilder()->SetInsertPoint(conditionBlock);
    if (!isInfinity)
    {
        auto conditionExpr = isSingleCondition ? conditions[0] : conditions[1];

        if (!conditionExpr->getType()->isBoolType())
        {
            return ErrorTable::addError(conditionExpr->getToken(), "Expected Boolean Type for Looping Condition");
        }

        this->getBuilder()->CreateCondBr(conditionExpr->codegen(this), bodyBlock, endBlock);
    }
    else
    {
        // If Infinity just jump to body block
        this->getBuilder()->CreateBr(bodyBlock);
    }

    // Block //
    // Set Insert Point to body block
    parentFun->getBasicBlockList().push_back(bodyBlock);
    this->getBuilder()->SetInsertPoint(bodyBlock);

    // Driver Body
    expr->getBody()->codegen(this);

    // Counting //
    // Jump to Counting
    this->getBuilder()->CreateBr(countBlock);

    // Set Insert Point to Counting
    parentFun->getBasicBlockList().push_back(countBlock);
    this->getBuilder()->SetInsertPoint(countBlock);

    // Check if counting expression found
    if (!isInfinity && !isSingleCondition)
    {
        auto countExpr = conditions[2];

        countExpr->codegen(this);
    }

    // Jump back to Condition
    this->getBuilder()->CreateBr(conditionBlock);

    // End Block //
    parentFun->getBasicBlockList().push_back(endBlock);
    this->getBuilder()->SetInsertPoint(endBlock);

    removeBreakBlock();
    removeContinueBlock();

    // Exit from statement
    exitScope();

    return nullptr;
}
