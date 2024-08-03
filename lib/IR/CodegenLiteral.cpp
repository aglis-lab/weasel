#include <math.h>

#include "weasel/IR/Codegen.h"

llvm::Value *Codegen::codegen(BoolLiteralExpression *expr)
{
    LOG(INFO) << "Codegen Boolean Literal Expression";

    return getBuilder()->getInt1(expr->getValue());
}

llvm::Value *Codegen::codegen(CharLiteralExpression *expr)
{
    LOG(INFO) << "Codegen Char Literal Expression";

    return getBuilder()->getInt8(expr->getValue());
}

llvm::Value *Codegen::codegen(NumberLiteralExpression *expr)
{
    LOG(INFO) << "Codegen Number Literal Expression";

    auto typeVal = expr->getType()->accept(this);
    return llvm::ConstantInt::get(typeVal, expr->getValue());
}

llvm::Value *Codegen::codegen(FloatLiteralExpression *expr)
{
    LOG(INFO) << "Codegen Float Literal Expression";

    auto floatTy = getBuilder()->getFloatTy();
    return llvm::ConstantFP::get(floatTy, expr->getValue());
}

llvm::Value *Codegen::codegen(DoubleLiteralExpression *expr)
{
    LOG(INFO) << "Codegen Double Literal Expression";

    auto doubleTy = getBuilder()->getDoubleTy();
    return llvm::ConstantFP::get(doubleTy, expr->getValue());
}

llvm::Value *Codegen::codegen(StringLiteralExpression *expr)
{
    LOG(INFO) << "Codegen String Literal Expression";

    auto *globalStringVariable = getBuilder()->CreateGlobalString(expr->getValue());
    std::vector<llvm::Value *> idxList = {
        this->getBuilder()->getInt64(0),
        this->getBuilder()->getInt64(0),
    };

    return llvm::ConstantExpr::getGetElementPtr(globalStringVariable->getValueType(), globalStringVariable, idxList, true);
}

llvm::Value *Codegen::codegen(ArrayLiteralExpression *expr)
{
    LOG(INFO) << "Codegen Array Literal";

    auto items = expr->getItems();
    auto numItem = items.size();
    auto itemType = expr->getType()->getContainedType()->accept(this);
    auto arrayType = llvm::ArrayType::get(itemType, numItem);
    auto valueNull = llvm::Constant::getNullValue(arrayType);
    auto valueArr = std::vector<llvm::Constant *>(numItem);
    auto i = 0;

    while (auto *c = valueNull->getAggregateElement(i))
    {
        if (auto *constVal = llvm::dyn_cast<llvm::Constant>(items[i]->accept(this)))
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
    auto gv = new llvm::GlobalVariable(*getModule(), arrayType, true, linkage, init);
    auto dataLayout = llvm::DataLayout(getModule());
    auto alignNum = dataLayout.getPrefTypeAlignment(arrayType);

    gv->setAlignment(llvm::Align(std::max((uint64_t)16, alignNum)));
    gv->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Local);

    return gv;
}

llvm::Value *Codegen::codegen(NilLiteralExpression *expr)
{
    LOG(INFO) << "Codegen Nil Literal Expression";

    auto typeV = expr->getType()->accept(this);
    return llvm::ConstantPointerNull::getNullValue(typeV);
}
