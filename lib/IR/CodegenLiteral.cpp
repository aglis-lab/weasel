#include "weasel/IR/Codegen.h"

llvm::Value *weasel::WeaselCodegen::codegen(BoolLiteralExpression *expr) const
{
    return getBuilder()->getInt1(expr->getValue());
}

llvm::Value *weasel::WeaselCodegen::codegen(CharLiteralExpression *expr) const
{
    return getBuilder()->getInt8(expr->getValue());
}

llvm::Value *weasel::WeaselCodegen::codegen(NumberLiteralExpression *expr)
{
    auto typeVal = expr->getType()->codegen(this);
    return llvm::ConstantInt::get(typeVal, expr->getValue());
}

llvm::Value *weasel::WeaselCodegen::codegen(FloatLiteralExpression *expr) const
{
    auto floatTy = getBuilder()->getFloatTy();
    return llvm::ConstantFP::get(floatTy, expr->getValue());
}

llvm::Value *weasel::WeaselCodegen::codegen(DoubleLiteralExpression *expr) const
{
    auto doubleTy = getBuilder()->getDoubleTy();
    return llvm::ConstantFP::get(doubleTy, expr->getValue());
}

llvm::Value *weasel::WeaselCodegen::codegen(StringLiteralExpression *expr) const
{
    auto *str = getBuilder()->CreateGlobalString(expr->getValue());
    std::vector<llvm::Value *> idxList;
    idxList.push_back(getBuilder()->getInt64(0));
    idxList.push_back(getBuilder()->getInt64(0));

    return llvm::ConstantExpr::getGetElementPtr(str->getType()->getElementType(), str, idxList, true);
}

llvm::Value *weasel::WeaselCodegen::codegen(ArrayLiteralExpression *expr)
{
    LOG(INFO) << "Codegen Array Literal";

    auto items = expr->getItems();
    auto numItem = items.size();
    auto itemType = expr->getType()->getContainedType()->codegen(this);
    auto arrayType = llvm::ArrayType::get(itemType, numItem);
    auto valueNull = llvm::Constant::getNullValue(arrayType);
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

    auto init = llvm::ConstantArray::get(arrayType, valueArr);
    auto linkage = llvm::GlobalVariable::LinkageTypes::PrivateLinkage;
    auto gv = new llvm::GlobalVariable(*getModule(), itemType, true, linkage, init);
    auto dataLayout = llvm::DataLayout(getModule());
    auto alignNum = dataLayout.getPrefTypeAlignment(arrayType);

    gv->setAlignment(llvm::Align(std::max((unsigned int)16, alignNum)));
    gv->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Local);

    return getBuilder()->CreateLoad(arrayType, gv);
}

llvm::Value *weasel::WeaselCodegen::codegen(NilLiteralExpression *expr)
{
    auto typeV = expr->getType()->codegen(this);
    return llvm::ConstantPointerNull::getNullValue(typeV);
}
