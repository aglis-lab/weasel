#include <weasel/AST/AST.h>
#include <weasel/IR/Codegen.h>

// Constructor for ArrayLiteral
weasel::ArrayLiteralExpression::ArrayLiteralExpression(std::vector<Expression *> items)
{
    // _items = items;

    // Type *itemType;
    // if (items.size() == 0)
    // {
    //     itemType = Type::getVoidType();
    // }
    // else
    // {
    //     itemType = items[0]->getType();
    // }

    // for (auto item : items)
    // {
    //     if (!item->getType()->isEqual(itemType))
    //     {
    //         LOG(ERROR) << "Array item's type isn't equal or different";
    //     }
    // }

    // _type = Type::getArrayType(itemType, items.size());
}

// Function
std::string weasel::Function::getManglingName()
{
    if (this->isExtern() || this->isMain())
    {
        return this->getIdentifier();
    }

    std::string mangleName;

    // Reserve 50 character
    mangleName.reserve(50);

    // First prefix
    mangleName += "_W";

    // Impl Struct
    if (this->isImplStructExist())
    {
        mangleName += "I" + this->getImplStruct()->getManglingName();
    }

    // Return Type
    mangleName += "R" + this->getType()->getManglingName();

    // Function Name with prefix
    mangleName += "7" + this->getIdentifier();

    // Arguments with prefix
    mangleName += "3";
    // for (auto item : this->getArguments())
    // {
    //     mangleName += item->getType()->getManglingName();
    // }

    // End of argument or function
    mangleName += "_";

    // Return those manglename
    return mangleName;
}

weasel::Function::~Function()
{
    // delete _body;

    // for (auto item : _arguments)
    // {
    //     delete item;
    // }

    // _arguments.clear();
}

// MethodCall Expression
weasel::MethodCallExpression::~MethodCallExpression()
{
    // delete _implExpression;
    // for (auto item : _args)
    // {
    //     delete item;
    // }

    // _args.clear();
}

// ArrayLiteralExpression Expression
weasel::ArrayLiteralExpression::~ArrayLiteralExpression()
{
    // _items.clear();
}

// LoopingStatement Expression
weasel::LoopingStatement::~LoopingStatement()
{
    // delete _body;

    // _conditions.clear();
}

// CompoundStatement Expression
weasel::CompoundStatement::~CompoundStatement()
{
    // _body.clear();
}

// DeclarationStatement Expression
weasel::DeclarationStatement::~DeclarationStatement()
{
    // delete _value;
}

// Unary Expression
weasel::UnaryExpression::~UnaryExpression()
{
    // delete _rhs;
}

// Return Expression
weasel::ReturnExpression::~ReturnExpression()
{
    // delete _value;
}

// Break Expression
weasel::BreakExpression::~BreakExpression()
{
    // delete _value;
}

// Continue Expression
weasel::ContinueExpression::~ContinueExpression()
{
    // delete _value;
}

// Call Expression
weasel::CallExpression::~CallExpression()
{
    // _args.clear();
}

// Array Expression
weasel::ArrayExpression::~ArrayExpression()
{
    // delete _indexExpr;
}

// Struct Expression
weasel::StructExpression::~StructExpression()
{
    // _fields.clear();
}

weasel::StructExpression::StructField::~StructField()
{
    // delete _expr;
}

// Field Expression
weasel::FieldExpression::~FieldExpression()
{
    // delete _parentField;
}

// Condition Expression
weasel::ConditionStatement::~ConditionStatement()
{
    // _conditions.clear();
    // _statements.clear();
}

// Expression
weasel::Expression::~Expression()
{
    // delete _type;
}

weasel::Token weasel::Expression::getToken() const { return _token; }

bool weasel::Expression::isNoType() const { return _type == nullptr; }
