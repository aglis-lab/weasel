#include <weasel/AST/AST.h>
#include <weasel/IR/Codegen.h>

// Expression give error
void weasel::Expression::makeError(Error *error)
{
    _error = error;
}

bool weasel::Expression::isError() const
{
    return _error != nullptr;
}

// Constructor for ArrayLiteral
weasel::ArrayLiteralExpression::ArrayLiteralExpression(std::vector<Expression *> items)
{
    _items = items;

    Type *itemType;
    if (items.size() == 0)
    {
        itemType = Type::getVoidType();
    }
    else
    {
        itemType = items[0]->getType();
    }

    for (auto item : items)
    {
        if (!item->getType()->isEqual(itemType))
        {
            LOG(ERROR) << "Array item's type isn't equal or different";
        }
    }

    _type = Type::getArrayType(itemType, items.size());
}

// Function
weasel::Function::~Function()
{
    _arguments.clear();

    delete _body;
}

// ArrayLiteralExpression Expression
weasel::ArrayLiteralExpression::~ArrayLiteralExpression()
{
    _items.clear();
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
