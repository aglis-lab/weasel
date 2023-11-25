#include <weasel/AST/AST.h>
#include <weasel/IR/Codegen.h>

llvm::Value *weasel::GlobalVariable::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::MethodCallExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

// Literal
llvm::Value *weasel::NumberLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::DoubleLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::FloatLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::BoolLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::CharLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ArrayLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::StringLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::NilLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

// Expression
llvm::Value *weasel::DeclarationStatement::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::VariableExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::FieldExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ArrayExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ArithmeticExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::LogicalExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::AssignmentExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ComparisonExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::CallExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ReturnExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::BreakExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ContinueExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::StructExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::Function::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

// Operator Expression
llvm::Value *weasel::UnaryExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::TypeCastExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

// Statement
llvm::Value *weasel::CompoundStatement::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::ConditionStatement::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *weasel::LoopingStatement::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

// Some Type Checking
bool weasel::Expression::isCompoundExpression()
{
    return dynamic_cast<weasel::CompoundStatement *>(this) != nullptr;
}
