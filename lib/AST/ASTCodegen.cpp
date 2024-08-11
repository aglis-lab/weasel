#include <weasel/AST/AST.h>
#include <weasel/IR/Codegen.h>

using namespace weasel;

#define UNIMPLEMENTED LOG(INFO) << "Unimplemented Codegen"

llvm::Value *LambdaStatement::accept(Codegen *c)
{
    UNIMPLEMENTED;
}

llvm::Value *ArgumentExpression::accept(Codegen *c)
{
    UNIMPLEMENTED;
}

llvm::Value *GlobalVariable::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *MethodCallExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *NumberLiteralExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *DoubleLiteralExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *FloatLiteralExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *BoolLiteralExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *CharLiteralExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *ArrayExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *StringLiteralExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *NilLiteralExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *DeclarationStatement::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *VariableExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *FieldExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *IndexExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *ArithmeticExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *LogicalExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *AssignmentExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *ComparisonExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *CallExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *ReturnExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *BreakExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *ContinueExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *StructExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *Function::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *UnaryExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *TypeCastExpression::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *CompoundStatement::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *ConditionStatement::accept(Codegen *c)
{
    return c->codegen(this);
}

llvm::Value *LoopingStatement::accept(Codegen *c)
{
    return c->codegen(this);
}
