#include "weasel/AST/AST.h"
#include "weasel/IR/Context.h"

// Literal
llvm::Value *weasel::NumberLiteralExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::FloatLiteralExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::BoolLiteralExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::CharLiteralExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ArrayLiteralExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::StringLiteralExpression::codegen(Context *c)
{
    return c->codegen(this);
}

// Statement
llvm::Value *weasel::DeclarationExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::StatementExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::VariableExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ArrayExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::BinaryOperatorExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::CallExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ReturnExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::NilLiteralExpression::codegen(Context *c)
{
    return c->codegen();
}

llvm::Function *weasel::Function::codegen(Context *c)
{
    return c->codegen(this);
}
