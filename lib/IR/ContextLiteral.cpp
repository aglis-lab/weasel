#include "llvm/IR/Constants.h"
#include "weasel/IR/Context.h"

llvm::Value *weasel::Context::codegen(BoolLiteralExpression *expr) const
{
    return getBuilder()->getInt1(expr->getValue());
}

llvm::Value *weasel::Context::codegen(CharLiteralExpression *expr) const
{
    return getBuilder()->getInt8(expr->getValue());
}

llvm::Value *weasel::Context::codegen(NumberLiteralExpression *expr)
{
    auto typeVal = expr->getType()->codegen(this);
    return llvm::ConstantInt::get(typeVal, expr->getValue());
}

llvm::Value *weasel::Context::codegen(FloatLiteralExpression *expr) const
{
    auto floatTy = getBuilder()->getFloatTy();
    return llvm::ConstantFP::get(floatTy, expr->getValue());
}

llvm::Value *weasel::Context::codegen(DoubleLiteralExpression *expr) const
{
    auto doubleTy = getBuilder()->getDoubleTy();
    return llvm::ConstantFP::get(doubleTy, expr->getValue());
}

llvm::Value *weasel::Context::codegen(StringLiteralExpression *expr) const
{
    auto *str = getBuilder()->CreateGlobalString(expr->getValue());
    std::vector<llvm::Value *> idxList;
    idxList.push_back(getBuilder()->getInt64(0));
    idxList.push_back(getBuilder()->getInt64(0));

    return llvm::ConstantExpr::getGetElementPtr(str->getType()->getElementType(), str, idxList, true);
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
    auto gv = new llvm::GlobalVariable(*getModule(), valueTy, true, linkage, init);
    auto dataLayout = llvm::DataLayout(getModule());
    auto alignNum = dataLayout.getPrefTypeAlignment(valueTy);

    gv->setAlignment(llvm::Align(std::max((unsigned int)16, alignNum)));
    gv->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Local);

    return getBuilder()->CreateLoad(gv->getType(), gv);
}

llvm::Value *weasel::Context::codegen(NilLiteralExpression *expr)
{
    auto typeV = expr->getType()->codegen(this);
    return llvm::ConstantPointerNull::getNullValue(typeV);
}
