#include "weasel/AST/AST.h"
#include "weasel/IR/Context.h"

// Literal
llvm::Value *weasel::NumberLiteralExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::DoubleLiteralExpression::codegen(Context *c)
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

llvm::Value *weasel::NilLiteralExpression::codegen(Context *c)
{
    return c->codegen(this);
}

// Expression
llvm::Value *weasel::DeclarationStatement::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::VariableExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::FieldExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ArrayExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ArithmeticExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::LogicalExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::AssignmentExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ComparisonExpression::codegen(Context *c)
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

llvm::Value *weasel::BreakExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ContinueExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::StructExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::Function::codegen(Context *c)
{
    return c->codegen(this);
}

// Operator Expression
llvm::Value *weasel::UnaryExpression::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::TypeCastExpression::codegen(Context *c)
{
    return c->codegen(this);
}

// Statement
llvm::Value *weasel::CompoundStatement::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ConditionStatement::codegen(Context *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::LoopingStatement::codegen(Context *c)
{
    return c->codegen(this);
}

// Some Type Checking
bool weasel::Expression::isCompoundExpression()
{
    return dynamic_cast<weasel::CompoundStatement *>(this) != nullptr;
}
