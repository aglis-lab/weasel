#include <weasel/Printer/Printer.h>
#include <weasel/Util/Util.h>

void weasel::Printer::print(weasel::Module *module)
{
    // // Print user types or struct
    // for (auto item : module->getUserTypes())
    // {
    //     item->print(this);

    //     // Newline after function declaration or definition
    //     fmt::println("");
    // }

    // Print it's function and it's body
    for (auto item : module->getFunctions())
    {
        item->print(this);

        // Newline after function declaration or definition
        fmt::println("");
    }
}

void weasel::Printer::print(weasel::Function *expr)
{
    std::string prefix = "@declare";
    char newlineOp = '\0';
    if (expr->getBody()->getBody().size() > 0)
    {
        prefix = "@define";
        newlineOp = ':';
    }

    std::string argStr;
    auto argSize = (int)expr->getArguments().size();
    for (int i = 0; i < argSize; i++)
    {
        auto item = expr->getArguments()[i];
        auto identifier = item->getArgumentName();
        argStr += fmt::format("{} {}", identifier, item->getType()->getTypeName());

        if (i != argSize - 1)
        {
            argStr += ", ";
        }
    }

    auto retStr = expr->getType()->getTypeName();
    auto identifier = expr->getIdentifier();

    fmt::println("{: >{}}{} {}({}) {}{}", "", this->getCurrentShift(), prefix, identifier, argStr, retStr, newlineOp);

    auto lastShift = this->getCurrentShift();
    this->setCurrentShift(lastShift + DEFAULT_SHIFT);

    expr->getBody()->print(this);

    this->setCurrentShift(lastShift);
}

void weasel::Printer::print(weasel::DeclarationStatement *expr)
{
    auto prefix = "@declare";
    if (expr->getValue() != nullptr)
    {
        prefix = "@define";
    }

    auto val = fmt::format("{} {} {}", prefix, expr->getIdentifier(), expr->getType()->getTypeName());
    fmt::print("{: >{}}{}", "", this->getCurrentShift(), val);
    if (expr->getValue() != nullptr)
    {
        fmt::print(" = ");

        expr->getValue()->printAsOperand(this);
    }

    fmt::println("");
}

void weasel::Printer::print(weasel::CompoundStatement *expr)
{
    for (auto item : expr->getBody())
    {
        item->print(this);
    }
}

void weasel::Printer::print(weasel::ConditionStatement *expr)
{
    int length = expr->getStatements().size();
    for (int i = 0; i < length; i++)
    {
        auto body = expr->getStatements()[i];

        if (i == length - 1 && expr->isElseExist())
        {
            fmt::println("{: >{}}else:", "", this->getCurrentShift());
        }
        else
        {
            auto condStatement = "if";
            auto cond = expr->getConditions()[i];

            if (i > 0)
            {
                condStatement = "else if";
            }

            fmt::print("{: >{}}{} ", "", this->getCurrentShift(), condStatement);
            cond->print(this);
            fmt::println(":");
        }

        auto lastShift = this->getCurrentShift();
        this->setCurrentShift(lastShift + DEFAULT_SHIFT);
        body->print(this);
        this->setCurrentShift(lastShift);
    }
}

void weasel::Printer::print(weasel::AssignmentExpression *expr)
{
    fmt::print("{: >{}}", "", this->getCurrentShift());
    expr->getLHS()->printAsOperand(this);
    fmt::print(" = ");
    expr->getRHS()->printAsOperand(this);
    fmt::println("");
}

void weasel::Printer::print(weasel::CallExpression *expr)
{
    fmt::print("{: >{}}", "", this->getCurrentShift());
    this->printAsOperand(expr);
    fmt::println("");
}

void weasel::Printer::print(weasel::ReturnExpression *expr)
{
    fmt::print("{: >{}}", "", this->getCurrentShift());
    this->printAsOperand(expr);
    fmt::println("");
}

void weasel::Printer::print(weasel::UnaryExpression *expr)
{
    this->printAsOperand(expr);
    fmt::println("");
}

void weasel::Printer::print(weasel::BoolLiteralExpression *expr)
{
    fmt::print("{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void weasel::Printer::print(weasel::NumberLiteralExpression *expr)
{
    fmt::print("{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void weasel::Printer::print(weasel::StringLiteralExpression *expr)
{
    auto val = expr->getValue();
    util::replaceInPlace(val, std::string("\n"), std::string("\\n"));
    fmt::print("\"{}\"", val);
}

void weasel::Printer::print(weasel::VariableExpression *expr)
{
    fmt::print("{} {}", expr->getIdentifier(), expr->getType()->getTypeName());
}

void weasel::Printer::print(weasel::ComparisonExpression *expr)
{
    this->printAsOperand(expr);
    fmt::println("");
}

//
// PRINT AS OPERAND //
// SIMPLE OPERAND WITHPUT NEWLINE INSTRUCTION //
//
void weasel::Printer::printAsOperand(weasel::NumberLiteralExpression *expr)
{
    fmt::print("{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void weasel::Printer::printAsOperand(weasel::DoubleLiteralExpression *expr)
{
    fmt::print("{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void weasel::Printer::printAsOperand(weasel::BoolLiteralExpression *expr)
{
    fmt::print("{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void weasel::Printer::printAsOperand(weasel::VariableExpression *expr)
{
    fmt::print("{} {}", expr->getIdentifier(), expr->getType()->getTypeName());
}

void weasel::Printer::printAsOperand(weasel::AssignmentExpression *expr)
{
    expr->getLHS()->printAsOperand(this);
    fmt::print(" = ");
    expr->getRHS()->printAsOperand(this);
}

void weasel::Printer::printAsOperand(weasel::ComparisonExpression *expr)
{
    expr->getLHS()->printAsOperand(this);
    fmt::print(" {} ", expr->getOperator().getValue());
    expr->getRHS()->printAsOperand(this);
}

void weasel::Printer::printAsOperand(weasel::CallExpression *expr)
{
    fmt::print("@call {}(", expr->getFunction()->getIdentifier());
    for (auto arg : expr->getArguments())
    {
        arg->printAsOperand(this);

        if (arg != expr->getArguments().back())
        {
            fmt::print(", ");
        }
    }
    fmt::print(")");
}

void weasel::Printer::printAsOperand(weasel::ReturnExpression *expr)
{
    fmt::print("return");
    if (!expr->getType()->isVoidType())
    {
        fmt::print(" ");
        expr->getValue()->print(this);
    }
}

void weasel::Printer::printAsOperand(weasel::UnaryExpression *expr)
{
    std::string op = "not-op";
    switch (expr->getOperator())
    {
    case UnaryExpression::Dereference:
        op = "*";
        break;
    case UnaryExpression::Negative:
        op = "-";
        break;
    case UnaryExpression::Positive:
        op = "+";
        break;
    case UnaryExpression::Not:
        op = "!";
        break;
    case UnaryExpression::Negation:
        op = "~";
        break;
    case UnaryExpression::Borrow:
        op = "&";
    }

    fmt::print(op);
    expr->getExpression()->printAsOperand(this);
}