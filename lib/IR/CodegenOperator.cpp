#include <iostream>

#include <llvm/IR/Function.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Module.h>

#include "weasel/IR/Codegen.h"

// Unimplemented
llvm::Value *weasel::WeaselCodegen::codegen(TypeCastExpression *expr)
{
    auto type = expr->getType();
    auto rhs = expr->getExpression();
    auto rhsType = rhs->getType();
    auto rhsVal = rhs->codegen(this);
    auto typeVal = type->codegen(this);

    if (
        rhsType->isPointerType() || type->isPointerType() ||
        rhsType->isArrayType() || type->isArrayType())
    {
        return getBuilder()->CreateBitOrPointerCast(rhsVal, typeVal);
    }

    if (type->isIntegerType() && (rhsType->isFloatType() || rhsType->isDoubleType()))
    {
        if (type->isSigned())
        {
            return getBuilder()->CreateFPToSI(rhsVal, typeVal);
        }
        else
        {
            return getBuilder()->CreateFPToUI(rhsVal, typeVal);
        }
    }

    if (rhsType->isIntegerType() && (type->isFloatType() || type->isDoubleType()))
    {
        if (rhsType->isSigned())
        {
            return getBuilder()->CreateSIToFP(rhsVal, typeVal);
        }
        else
        {
            return getBuilder()->CreateUIToFP(rhsVal, typeVal);
        }
    }

    assert(false && "Type Casting not supported");

    return nullptr;
}

llvm::Value *weasel::WeaselCodegen::codegen(ArithmeticExpression *expr)
{
    auto opToken = expr->getOperator();
    auto lhs = expr->getLHS();
    auto rhs = expr->getRHS();
    auto lhsType = lhs->getType();
    auto rhsType = rhs->getType();
    auto exprType = expr->getType();

    assert(lhsType->isEqual(rhsType) && "Data type look different");

    auto lhsVal = lhs->codegen(this);
    auto rhsVal = rhs->codegen(this);
    auto isFloat = lhsType->isFloatType() || lhsType->isDoubleType();
    auto isSigned = exprType->isSigned();

    if (exprType->isIntegerType())
    {
        auto lhsTypeV = lhsType->codegen(this);

        rhsVal = castInteger(rhsVal, lhsTypeV, isSigned);
    }

    if (isFloat)
    {
        switch (opToken.getTokenKind())
        {
        case TokenKind::TokenOperatorStar:
            return getBuilder()->CreateFMul(lhsVal, rhsVal);
        case TokenKind::TokenOperatorSlash:
            return getBuilder()->CreateFDiv(lhsVal, rhsVal);
        case TokenKind::TokenOperatorPercent:
            return getBuilder()->CreateFRem(lhsVal, rhsVal);
        case TokenKind::TokenOperatorPlus:
            return getBuilder()->CreateFAdd(lhsVal, rhsVal);
        case TokenKind::TokenOperatorNegative:
            return getBuilder()->CreateFAdd(lhsVal, rhsVal);
        case TokenKind::TokenOperatorOr:
        case TokenKind::TokenOperatorAnd:
        case TokenKind::TokenOperatorCaret:
        default:
            LOG(INFO) << expr->getToken().getValue();
            assert(false && "Invalid operator for arithmetic expression");
        }
    }

    switch (opToken.getTokenKind())
    {
    case TokenKind::TokenOperatorStar:
    {
        if (isSigned)
        {
            return getBuilder()->CreateNSWMul(lhsVal, rhsVal);
        }
        return getBuilder()->CreateNUWMul(lhsVal, rhsVal);
    }
    case TokenKind::TokenOperatorSlash:
    {
        if (isSigned)
        {
            return getBuilder()->CreateSDiv(lhsVal, rhsVal);
        }

        return getBuilder()->CreateUDiv(lhsVal, rhsVal);
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
            return getBuilder()->CreateNSWAdd(lhsVal, rhsVal);
        }
        return getBuilder()->CreateNUWAdd(lhsVal, rhsVal);
    }
    case TokenKind::TokenOperatorCaret:
    {
        return getBuilder()->CreateXor(lhsVal, rhsVal);
    }
    case TokenKind::TokenOperatorNegative:
    {
        if (isSigned)
        {
            return getBuilder()->CreateNSWSub(lhsVal, rhsVal);
        }
        return getBuilder()->CreateNUWSub(lhsVal, rhsVal);
    }
    case TokenKind::TokenOperatorAnd:
    {
        return getBuilder()->CreateAnd(lhsVal, rhsVal);
    }
    case TokenKind::TokenOperatorOr:
    {
        return getBuilder()->CreateOr(lhsVal, rhsVal);
    }
    case TokenKind::TokenOperatorShiftRight:
    {
        return getBuilder()->CreateAShr(lhsVal, rhsVal);
    }
    case TokenKind::TokenOperatorShiftLeft:
    {
        return getBuilder()->CreateShl(lhsVal, rhsVal);
    }
    default:
        LOG(INFO) << expr->getToken().getValue();
        assert(false && "Invalid operator for arithmetic expression");
    }
}

// TODO: Understanding Logical Operator
// && ||
llvm::Value *weasel::WeaselCodegen::codegen(LogicalExpression *expr)
{
    auto lhs = expr->getLHS();
    auto rhs = expr->getRHS();
    auto lhsType = lhs->getType();
    auto rhsType = rhs->getType();
    auto exprType = expr->getType();

    assert(lhsType->isEqual(rhsType) && "Data type look different");

    auto rhsVal = rhs->codegen(this);
    auto isSigned = exprType->isSigned();

    if (exprType->isIntegerType())
    {
        auto lhsTypeV = lhsType->codegen(this);

        rhsVal = castInteger(rhsVal, lhsTypeV, isSigned);
    }

    return nullptr;
}

llvm::Value *weasel::WeaselCodegen::codegen(AssignmentExpression *expr)
{
    auto lhs = expr->getLHS();
    auto rhs = expr->getRHS();
    auto lhsType = lhs->getType();
    auto rhsType = rhs->getType();

    assert(lhsType->isEqual(rhsType) && "Data type look different");

    // Codegen RHS First
    // Because RHS may depend on LHS because some reason
    rhs->setAccess(AccessID::Load);
    lhs->setAccess(AccessID::Allocation);

    auto rhsVal = rhs->codegen(this);
    auto lhsVal = lhs->codegen(this);

    // Casting Integer
    if (rhsType->isIntegerType())
    {
        auto lhsTypeV = lhsType->codegen(this);
        rhsVal = castInteger(rhsVal, lhsTypeV, lhsType->isSigned());
    }

    if (rhsType->isStructType())
    {
        auto rhsTypeStruct = dynamic_cast<StructType *>(rhsType.get());
        getBuilder()->CreateMemCpy(lhsVal, llvm::MaybeAlign(4), rhsVal, llvm::MaybeAlign(4), rhsTypeStruct->getTypeWidthByte());
    }
    else
    {
        getBuilder()->CreateStore(rhsVal, lhsVal);
    }

    if (expr->isAccessAllocation())
    {
        return lhsVal;
    }

    auto lhsTypeV = lhsType->codegen(this);
    return getBuilder()->CreateLoad(lhsTypeV, lhsVal);
}

llvm::Value *weasel::WeaselCodegen::codegen(ComparisonExpression *expr)
{
    auto opToken = expr->getOperator();
    auto lhs = expr->getLHS();
    auto rhs = expr->getRHS();
    auto lhsType = lhs->getType();
    auto rhsType = rhs->getType();
    auto exprType = expr->getType();

    assert(lhsType->isEqual(rhsType) && "Data type look different");

    lhs->setAccess(AccessID::Load);
    rhs->setAccess(AccessID::Load);

    auto lhsVal = lhs->codegen(this);
    auto rhsVal = rhs->codegen(this);
    auto isSigned = exprType->isSigned();
    auto isFloat = lhsType->isFloatType() || lhsType->isDoubleType();

    // Operator
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
        LOG(ERROR) << "Unimplemented operator " + opToken.getValue();
        assert(false && "Unimplemented operator");
    }
    }
}

llvm::Value *weasel::WeaselCodegen::codegen(UnaryExpression *expr)
{
    auto op = expr->getOperator();
    auto rhs = expr->getExpression();
    auto rhsType = rhs->getType();
    auto rhsTypeVal = rhsType->codegen(this);

    if (op == UnaryExpression::Borrow)
    {
        rhs->setAccess(AccessID::Allocation);
        return rhs->codegen(this);
    }

    auto rhsVal = rhs->codegen(this);
    if (op == UnaryExpression::Dereference)
    {
        if (expr->isAccessAllocation())
        {
            return rhsVal;
        }

        auto typeV = rhsType->getContainedType()->codegen(this);
        return getBuilder()->CreateLoad(typeV, rhsVal);
    }

    if (op == UnaryExpression::Negative)
    {
        if (rhsType->isIntegerType())
        {
            auto zeroVal = getBuilder()->getInt32(0);

            if (rhsType->getTypeWidth() < 32)
            {
                rhsVal = getBuilder()->CreateSExt(rhsVal, getBuilder()->getInt32Ty());
            }
            else if (rhsType->getTypeWidth() > 32)
            {
                zeroVal = getBuilder()->getInt64(0);
            }

            llvm::Value *val;
            if (rhsType->isSigned())
            {
                val = getBuilder()->CreateNSWSub(zeroVal, rhsVal);
            }
            else
            {
                val = getBuilder()->CreateSub(zeroVal, rhsVal);
            }

            return getBuilder()->CreateSExtOrTrunc(val, rhsTypeVal);
        }

        if (rhsType->isFloatType() || rhsType->isDoubleType())
        {
            auto zeroVal = llvm::ConstantFP::get(rhsTypeVal, 0);

            return getBuilder()->CreateFSub(zeroVal, rhsVal);
        }
    }

    if (op == UnaryExpression::Not)
    {
        llvm::Value *val;
        if (rhsType->isIntegerType())
        {
            val = getBuilder()->CreateICmpNE(rhsVal, llvm::ConstantInt::get(rhsTypeVal, 0));
        }

        if (rhsType->isFloatType() || rhsType->isDoubleType())
        {
            val = getBuilder()->CreateFCmpUNE(rhsVal, llvm::ConstantFP::get(rhsTypeVal, 0));
        }

        assert(val && "Unary Expression NOT is not valid");

        return getBuilder()->CreateXor(val, 1);
    }

    if (op == UnaryExpression::Positive)
    {
        return rhsVal;
    }

    if (op == UnaryExpression::Negation && rhsType->isIntegerType())
    {
        return getBuilder()->CreateXor(rhsVal, -1);
    }

    LOG(ERROR) << "Unary Expression is not valid " << expr->getToken().getValue();

    assert(false && "Unary Expression is not valid");

    return nullptr;
}
