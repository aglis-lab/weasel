#include "weasel/Printer/Printer.h"
#include "weasel/Util/Util.h"

using namespace weasel;

void Printer::print(GlobalVariable *expr)
{
    printAsOperand(expr);
    fmt::println("");
}

void Printer::printAsOperand(GlobalVariable *expr)
{
    fmt::print(_out, "@define {} {} = ", expr->getToken().getValue(), expr->getIdentifier());
    expr->getValue()->printAsOperand(this);
}

void Printer::print(Module *module)
{
    // Print user types or struct
    for (auto item : module->getUserTypes())
    {
        fmt::println(_out, "@type {}:", item->getIdentifier());

        auto fieldShift = getCurrentShift() + DEFAULT_SHIFT;
        for (auto field : item->getFields())
        {
            fmt::println(_out, "{: >{}}{} {}", "", fieldShift, field.getIdentifier(), field.getType()->getTypeName());
        }

        // Newline after function declaration or definition
        fmt::println(_out, "");
    }

    // Print it's function and it's body
    for (auto item : module->getFunctions())
    {
        item->print(this);

        // Newline after function declaration or definition
        fmt::println(_out, "");
    }
}

void Printer::print(MethodCallExpression *expr)
{
    printAsOperand(expr);
    fmt::println("");
}

void Printer::printAsOperand(MethodCallExpression *expr)
{
    fmt::print("no implemented yet");
}

void Printer::print(Function *expr)
{
    std::string prefix = "@declare";
    auto newlineOp = "";
    if (expr->getBody() && expr->getBody()->getBody().size() > 0)
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

    fmt::println(_out, "{: >{}}{} {}({}) {}{}", "", getCurrentShift(), prefix, identifier, argStr, retStr, newlineOp);

    auto lastShift = getCurrentShift();
    setCurrentShift(lastShift + DEFAULT_SHIFT);

    if (expr->getBody())
    {
        expr->getBody()->print(this);
    }

    setCurrentShift(lastShift);
}

void Printer::print(ArrayLiteralExpression *expr)
{
    fmt::print(_out, "{: >{}}", "", getCurrentShift());
    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(ArithmeticExpression *expr)
{
    fmt::print(_out, "{: >{}}", "", getCurrentShift());
    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(DeclarationStatement *expr)
{
    fmt::print(_out, "{: >{}}", "", getCurrentShift());
    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(LoopingStatement *expr)
{
    auto conditions = expr->getConditions();

    fmt::print(_out, "{: >{}}for ", "", getCurrentShift());
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

    auto lastShift = getCurrentShift();
    setCurrentShift(lastShift + DEFAULT_SHIFT);
    expr->getBody()->print(this);
    setCurrentShift(lastShift);
}

void Printer::print(CompoundStatement *expr)
{
    for (auto item : expr->getBody())
    {
        item->print(this);
    }
}

void Printer::print(ConditionStatement *expr)
{
    int length = expr->getStatements().size();
    for (int i = 0; i < length; i++)
    {
        auto body = expr->getStatements()[i];

        if (i == length - 1 && expr->isElseExist())
        {
            fmt::println(_out, "{: >{}}else:", "", getCurrentShift());
        }
        else
        {
            auto condStatement = "if";
            auto cond = expr->getConditions()[i];

            if (i > 0)
            {
                condStatement = "else if";
            }

            fmt::print(_out, "{: >{}}{} ", "", getCurrentShift(), condStatement);
            cond->printAsOperand(this);
            fmt::println(_out, ":");
        }

        auto lastShift = getCurrentShift();
        setCurrentShift(lastShift + DEFAULT_SHIFT);
        body->print(this);
        setCurrentShift(lastShift);
    }
}

void Printer::print(FieldExpression *expr)
{
    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(ArrayExpression *expr)
{
    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(AssignmentExpression *expr)
{
    fmt::print(_out, "{: >{}}", "", getCurrentShift());
    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, " = ");
    expr->getRHS()->printAsOperand(this);
    fmt::println(_out, "");
}

void Printer::print(CallExpression *expr)
{
    fmt::print(_out, "{: >{}}", "", getCurrentShift());
    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(ReturnExpression *expr)
{
    fmt::print(_out, "{: >{}}", "", getCurrentShift());
    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(UnaryExpression *expr)
{
    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(BoolLiteralExpression *expr)
{
    fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void Printer::print(NumberLiteralExpression *expr)
{
    fmt::println(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void Printer::print(StringLiteralExpression *expr)
{
    auto val = expr->getValue();
    util::replaceInPlace(val, std::string("\n"), std::string("\\n"));
    fmt::print(_out, "\"{}\"", val);
}

void Printer::print(VariableExpression *expr)
{
    fmt::print(_out, "{} {}", expr->getIdentifier(), expr->getType()->getTypeName());
}

void Printer::print(ComparisonExpression *expr)
{
    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(StructExpression *expr)
{
    printAsOperand(expr);
    fmt::println(_out, "");
}

//
// PRINT AS OPERAND //
// SIMPLE OPERAND WITHPUT NEWLINE INSTRUCTION //
//
void Printer::printAsOperand(StructExpression *expr)
{
    // fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
    fmt::print(_out, "StructExpression : Not Implemented Yet");
}

void Printer::printAsOperand(NumberLiteralExpression *expr)
{
    fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void Printer::printAsOperand(DoubleLiteralExpression *expr)
{
    fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void Printer::printAsOperand(BoolLiteralExpression *expr)
{
    fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void Printer::printAsOperand(VariableExpression *expr)
{
    fmt::print(_out, "{} {}", expr->getIdentifier(), expr->getType()->getTypeName());
}

void Printer::printAsOperand(AssignmentExpression *expr)
{
    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, " = ");
    expr->getRHS()->printAsOperand(this);
}

void Printer::printAsOperand(ComparisonExpression *expr)
{
    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, " {} ", expr->getOperator().getValue());
    expr->getRHS()->printAsOperand(this);
}

void Printer::printAsOperand(CallExpression *expr)
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

void Printer::printAsOperand(FieldExpression *expr)
{
    auto varExpr = dynamic_cast<VariableExpression *>(expr->getParentField());
    fmt::print(_out, "{}.{} {}", varExpr->getIdentifier(), expr->getIdentifier(), expr->getType()->getTypeName());
}

void Printer::printAsOperand(ArrayExpression *expr)
{
    fmt::print(_out, "{}[", expr->getIdentifier());
    expr->getIndex()->printAsOperand(this);
    fmt::print(_out, "]");
}

void Printer::printAsOperand(ReturnExpression *expr)
{
    fmt::print(_out, "return");
    if (!expr->getType()->isVoidType())
    {
        fmt::print(_out, " ");
        expr->getValue()->print(this);
    }
}

void Printer::printAsOperand(UnaryExpression *expr)
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

    fmt::print(_out, "{}", op);
    expr->getExpression()->printAsOperand(this);
}

void Printer::printAsOperand(DeclarationStatement *expr)
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

void Printer::printAsOperand(ArithmeticExpression *expr)
{
    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, " {} ", expr->getOperator().getValue());
    expr->getRHS()->printAsOperand(this);
}

void Printer::printAsOperand(StringLiteralExpression *expr)
{
    auto val = util::normalizeStringLiteral(expr->getValue());
    fmt::print(_out, "\"{}\"", val);
}

void Printer::printAsOperand(CharLiteralExpression *expr)
{
    auto val = util::normalizeStringLiteral(std::string(1, expr->getValue()));
    fmt::print(_out, "\'{}\'", val);
}

void Printer::printAsOperand(ArrayLiteralExpression *expr)
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
