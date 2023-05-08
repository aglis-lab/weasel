#include <weasel/AST/AST.h>
#include <weasel/IR/Codegen.h>

// Function
weasel::Function::~Function()
{
    _arguments.clear();

    delete _body;
}

// LoopingStatement Expression
weasel::LoopingStatement::~LoopingStatement()
{
    delete _body;

    _conditions.clear();
}

// CompoundStatement Expression
weasel::CompoundStatement::~CompoundStatement()
{
    _body.clear();
}

// DeclarationStatement Expression
weasel::DeclarationStatement::~DeclarationStatement()
{
    delete _value;
}

// Unary Expression
weasel::UnaryExpression::~UnaryExpression()
{
    delete _rhs;
}

// Return Expression
weasel::ReturnExpression::~ReturnExpression()
{
    delete _value;
}

// Break Expression
weasel::BreakExpression::~BreakExpression()
{
    delete _value;
}

// Continue Expression
weasel::ContinueExpression::~ContinueExpression()
{
    delete _value;
}

// Call Expression
weasel::CallExpression::~CallExpression()
{
    _args.clear();
}

// Array Expression
weasel::ArrayExpression::~ArrayExpression()
{
    delete _indexExpr;
}

// Struct Expression
weasel::StructExpression::~StructExpression()
{
    _fields.clear();
}

weasel::StructExpression::StructField::~StructField()
{
    delete _expr;
}

// Field Expression
weasel::FieldExpression::~FieldExpression()
{
    delete _parentField;
}

// Condition Expression
weasel::ConditionStatement::~ConditionStatement()
{
    _conditions.clear();
    _statements.clear();
}

// Expression
weasel::Expression::~Expression()
{
    delete _type;
}

weasel::Token weasel::Expression::getToken() const { return _token; }

weasel::Type *weasel::Expression::getType() const { return _type; }

void weasel::Expression::setType(Type *type) { _type = type; }

bool weasel::Expression::isNoType() const { return _type == nullptr; }

bool weasel::Expression::isRHS()
{
    for (auto item : _metas)
    {
        if (item == MetaID::RHS)
        {
            return true;
        }
    }

    return false;
}

bool weasel::Expression::isLHS()
{
    for (auto item : _metas)
    {
        if (item == MetaID::LHS)
        {
            return true;
        }
    }

    return false;
}
