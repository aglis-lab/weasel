#include <weasel/AST/AST.h>
#include <weasel/IR/Codegen.h>

using namespace weasel;

#define UNIMPLEMENTED LOG(INFO) << "Unimplemented Codegen"

llvm::Value *ArgumentExpression::accept(WeaselCodegen *c)
{
    UNIMPLEMENTED;
}

llvm::Value *GlobalVariable::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *MethodCallExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *NumberLiteralExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *DoubleLiteralExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *FloatLiteralExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *BoolLiteralExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *CharLiteralExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *ArrayLiteralExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *StringLiteralExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *NilLiteralExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *DeclarationStatement::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *VariableExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *FieldExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *ArrayExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *ArithmeticExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *LogicalExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *AssignmentExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *ComparisonExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *CallExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *ReturnExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *BreakExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *ContinueExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *StructExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *Function::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *UnaryExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *TypeCastExpression::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *CompoundStatement::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *ConditionStatement::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}

llvm::Value *LoopingStatement::accept(WeaselCodegen *c)
{
    return c->codegen(this);
}
