#include <weasel/AST/AST.h>
#include <weasel/IR/Codegen.h>

using namespace weasel;

#define UNIMPLEMENTED LOG(INFO) << "Unimplemented Codegen"

llvm::Value *ArgumentExpression::codegen(WeaselCodegen *c)
{
    UNIMPLEMENTED;
}

llvm::Value *GlobalVariable::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *MethodCallExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *NumberLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *DoubleLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *FloatLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *BoolLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *CharLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *ArrayLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *StringLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *NilLiteralExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *DeclarationStatement::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *VariableExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *FieldExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *ArrayExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *ArithmeticExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *LogicalExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *AssignmentExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *ComparisonExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *CallExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *ReturnExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *BreakExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *ContinueExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *StructExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *Function::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *UnaryExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *TypeCastExpression::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *CompoundStatement::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *ConditionStatement::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *LoopingStatement::codegen(WeaselCodegen *c)
{
    return c->codegen(this);
}
