#include "weasel/Printer/Printer.h"
#include "weasel/Util/Util.h"

void weasel::Printer::print(weasel::GlobalVariable *expr)
{
    this->printAsOperand(expr);
    fmt::println("");
}

void weasel::Printer::printAsOperand(weasel::GlobalVariable *expr)
{
    fmt::print(_out, "@define {} {} = ", expr->getToken().getValue(), expr->getIdentifier());
    expr->getValue()->printAsOperand(this);
}

void weasel::Printer::print(weasel::Module *module)
{
    // // Print user types or struct
    // for (auto item : module->getUserTypes())
    // {
    //     fmt::println(_out, "@declare ", item->getIdentifier());

    //     // Newline after function declaration or definition
    //     fmt::println(_out, "");
    // }

    // Print it's function and it's body
    for (auto item : module->getFunctions())
    {
        item->print(this);

        // Newline after function declaration or definition
        fmt::println(_out, "");
    }
}

void weasel::Printer::print(weasel::MethodCallExpression *expr)
{
    this->printAsOperand(expr);
    fmt::println("");
}

void weasel::Printer::printAsOperand(weasel::MethodCallExpression *expr)
{
    fmt::print("no implemented yet");
}

void weasel::Printer::print(weasel::Function *expr)
{
    std::string prefix = "@declare";
    auto newlineOp = "";
    if (expr->getBody()->getBody().size() > 0)
    {
        prefix = "@define";
        newlineOp = ":";
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

    fmt::println(_out, "{: >{}}{} {}({}) {}{}", "", this->getCurrentShift(), prefix, identifier, argStr, retStr, newlineOp);

    auto lastShift = this->getCurrentShift();
    this->setCurrentShift(lastShift + DEFAULT_SHIFT);

    expr->getBody()->print(this);

    this->setCurrentShift(lastShift);
}

void weasel::Printer::print(weasel::ArrayLiteralExpression *expr)
{
    fmt::print(_out, "{: >{}}", "", this->getCurrentShift());
    this->printAsOperand(expr);
    fmt::println(_out, "");
}

void weasel::Printer::print(weasel::ArithmeticExpression *expr)
{
    fmt::print(_out, "{: >{}}", "", this->getCurrentShift());
    this->printAsOperand(expr);
    fmt::println(_out, "");
}

void weasel::Printer::print(weasel::DeclarationStatement *expr)
{
    fmt::print(_out, "{: >{}}", "", this->getCurrentShift());
    this->printAsOperand(expr);
    fmt::println(_out, "");
}

void weasel::Printer::print(weasel::LoopingStatement *expr)
{
    auto conditions = expr->getConditions();

    fmt::print(_out, "{: >{}}for ", "", this->getCurrentShift());
    for (int i = 0; i < (int)conditions.size(); i++)
    {
        auto isLastElement = i == (int)conditions.size() - 1;
        auto cond = conditions[i];
        if (cond == nullptr)
        {
            if (!isLastElement)
            {
                fmt::print(_out, "; ");
            }

            continue;
        }

        cond->printAsOperand(this);
        if (!isLastElement)
        {
            fmt::print(_out, "; ");
        }
    }

    fmt::println(_out, ":");

    auto lastShift = this->getCurrentShift();
    this->setCurrentShift(lastShift + DEFAULT_SHIFT);
    expr->getBody()->print(this);
    this->setCurrentShift(lastShift);
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
            fmt::println(_out, "{: >{}}else:", "", this->getCurrentShift());
        }
        else
        {
            auto condStatement = "if";
            auto cond = expr->getConditions()[i];

            if (i > 0)
            {
                condStatement = "else if";
            }

            fmt::print(_out, "{: >{}}{} ", "", this->getCurrentShift(), condStatement);
            cond->printAsOperand(this);
            fmt::println(_out, ":");
        }

        auto lastShift = this->getCurrentShift();
        this->setCurrentShift(lastShift + DEFAULT_SHIFT);
        body->print(this);
        this->setCurrentShift(lastShift);
    }
}

void weasel::Printer::print(weasel::FieldExpression *expr)
{
    this->printAsOperand(expr);
    fmt::println(_out, "");
}

void weasel::Printer::print(weasel::ArrayExpression *expr)
{
    this->printAsOperand(expr);
    fmt::println(_out, "");
}

void weasel::Printer::print(weasel::AssignmentExpression *expr)
{
    fmt::print(_out, "{: >{}}", "", this->getCurrentShift());
    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, " = ");
    expr->getRHS()->printAsOperand(this);
    fmt::println(_out, "");
}

void weasel::Printer::print(weasel::CallExpression *expr)
{
    fmt::print(_out, "{: >{}}", "", this->getCurrentShift());
    this->printAsOperand(expr);
    fmt::println(_out, "");
}

void weasel::Printer::print(weasel::ReturnExpression *expr)
{
    fmt::print(_out, "{: >{}}", "", this->getCurrentShift());
    this->printAsOperand(expr);
    fmt::println(_out, "");
}

void weasel::Printer::print(weasel::UnaryExpression *expr)
{
    this->printAsOperand(expr);
    fmt::println(_out, "");
}

void weasel::Printer::print(weasel::BoolLiteralExpression *expr)
{
    fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void weasel::Printer::print(weasel::NumberLiteralExpression *expr)
{
    fmt::println(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void weasel::Printer::print(weasel::StringLiteralExpression *expr)
{
    auto val = expr->getValue();
    util::replaceInPlace(val, std::string("\n"), std::string("\\n"));
    fmt::print(_out, "\"{}\"", val);
}

void weasel::Printer::print(weasel::VariableExpression *expr)
{
    fmt::print(_out, "{} {}", expr->getIdentifier(), expr->getType()->getTypeName());
}

void weasel::Printer::print(weasel::ComparisonExpression *expr)
{
    this->printAsOperand(expr);
    fmt::println(_out, "");
}

void weasel::Printer::print(weasel::StructExpression *expr)
{
    this->printAsOperand(expr);
    fmt::println(_out, "");
}

//
// PRINT AS OPERAND //
// SIMPLE OPERAND WITHPUT NEWLINE INSTRUCTION //
//
void weasel::Printer::printAsOperand(weasel::StructExpression *expr)
{
    // fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
    fmt::print(_out, "StructExpression : Not Implemented Yet");
}

void weasel::Printer::printAsOperand(weasel::NumberLiteralExpression *expr)
{
    fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void weasel::Printer::printAsOperand(weasel::DoubleLiteralExpression *expr)
{
    fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void weasel::Printer::printAsOperand(weasel::BoolLiteralExpression *expr)
{
    fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void weasel::Printer::printAsOperand(weasel::VariableExpression *expr)
{
    fmt::print(_out, "{} {}", expr->getIdentifier(), expr->getType()->getTypeName());
}

void weasel::Printer::printAsOperand(weasel::AssignmentExpression *expr)
{
    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, " = ");
    expr->getRHS()->printAsOperand(this);
}

void weasel::Printer::printAsOperand(weasel::ComparisonExpression *expr)
{
    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, " {} ", expr->getOperator().getValue());
    expr->getRHS()->printAsOperand(this);
}

void weasel::Printer::printAsOperand(weasel::CallExpression *expr)
{
    fmt::print(_out, "@call {}(", expr->getFunction()->getIdentifier());
    for (auto arg : expr->getArguments())
    {
        arg->printAsOperand(this);

        if (arg != expr->getArguments().back())
        {
            fmt::print(_out, ", ");
        }
    }
    fmt::print(_out, ") {}", expr->getType()->getTypeName());
}

void weasel::Printer::printAsOperand(weasel::FieldExpression *expr)
{
    auto varExpr = dynamic_cast<VariableExpression *>(expr->getParentField());
    fmt::print(_out, "{}.{} {}", varExpr->getIdentifier(), expr->getField(), expr->getType()->getTypeName());
}

void weasel::Printer::printAsOperand(weasel::ArrayExpression *expr)
{
    fmt::print(_out, "{}[", expr->getIdentifier());
    expr->getIndex()->printAsOperand(this);
    fmt::print(_out, "]");
}

void weasel::Printer::printAsOperand(weasel::ReturnExpression *expr)
{
    fmt::print(_out, "return");
    if (!expr->getType()->isVoidType())
    {
        fmt::print(_out, " ");
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

    fmt::print(_out, op);
    expr->getExpression()->printAsOperand(this);
}

void weasel::Printer::printAsOperand(weasel::DeclarationStatement *expr)
{
    auto prefix = "@declare";
    if (expr->getValue() != nullptr)
    {
        prefix = "@define";
    }

    fmt::print(_out, "{} {} {}", prefix, expr->getIdentifier(), expr->getType()->getTypeName());
    if (expr->getValue() != nullptr)
    {
        fmt::print(_out, " = ");

        expr->getValue()->printAsOperand(this);
    }
}

void weasel::Printer::printAsOperand(weasel::ArithmeticExpression *expr)
{
    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, " {} ", expr->getOperator().getValue());
    expr->getRHS()->printAsOperand(this);
}

void weasel::Printer::printAsOperand(weasel::StringLiteralExpression *expr)
{
    auto val = util::normalizeStringLiteral(expr->getValue());
    fmt::print(_out, "\"{}\"", val);
}

void weasel::Printer::printAsOperand(weasel::CharLiteralExpression *expr)
{
    auto val = util::normalizeStringLiteral(std::string(1, expr->getValue()));
    fmt::print(_out, "\'{}\'", val);
}

void weasel::Printer::printAsOperand(weasel::ArrayLiteralExpression *expr)
{
    auto items = expr->getItems();
    auto count = (int)items.size();

    fmt::print("[");
    for (int i = 0; i < count; i++)
    {
        items[i]->printAsOperand(this);

        if (i != count - 1)
        {
            fmt::print(", ");
        }
    }
    fmt::print("]");
}
