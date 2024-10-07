#include "weasel/Printer/Printer.h"
#include "weasel/Util/Util.h"

using namespace weasel;

#define PRINT_OPERAND(X) LOG(INFO) << "Print As Operantd " << X
#define PRINT(X) LOG(INFO) << "Print Expression " << X

void Printer::print(PackageHandle package)
{
    PRINT("Package");

    // Print Global Value
    for (auto &item : package->getGlobalVariables())
    {
        item->print(this);
    }

    // Print user types or struct
    for (auto item : package->getUserTypes())
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
    for (auto item : package->getFunctions())
    {
        item->print(this);

        // Newline after function declaration or definition
        fmt::println(_out, "");
    }
}

void Printer::print(GlobalVariable *expr)
{
    PRINT("GlobalVariable");

    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(BreakExpression *expr)
{
    PRINT("BreakExpression");

    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(MethodCallExpression *expr)
{
    PRINT("MethodCallExpression");

    printAsOperand(expr);
    fmt::println("");
}

void Printer::print(Function *expr)
{
    PRINT("Function");

    // Attribute
    string attr;
    if (expr->getFunctionType()->getIstatic())
    {
        attr = "static ";
    }

    if (attr.size() > 0)
    {
        attr.pop_back();
    }

    fmt::println(_out, "@attr[{}]", attr);

    // Declare and define Function
    std::string prefix = "@declare";
    auto newlineOp = "";
    if (expr->getBody() && expr->getBody()->getBody().size() > 0)
    {
        prefix = "@define";
        newlineOp = ":";
    }

    if (expr->isImplTypeExist())
    {
        prefix = fmt::format("@impl {}", expr->getImplType()->getTypeName());
    }

    std::string argStr;
    auto argSize = (int)expr->getArguments().size();
    for (int i = 0; i < argSize; i++)
    {
        auto item = expr->getArguments()[i];
        auto identifier = item->getIdentifier();
        if (item->isImplThis())
        {
            identifier = "this";
        }

        argStr += fmt::format("{} {}", identifier, item->getType()->getTypeName());
        if (i != argSize - 1)
        {
            argStr += ", ";
        }
    }

    auto retStr = expr->getReturnType()->getTypeName();
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

void Printer::print(ArrayExpression *expr)
{
    PRINT("ArrayLiteralExpression");

    fmt::print(_out, "{: >{}}", "", getCurrentShift());
    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(ArithmeticExpression *expr)
{
    PRINT("ArithmeticExpression");

    fmt::print(_out, "{: >{}}", "", getCurrentShift());
    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(DeclarationStatement *expr)
{
    PRINT("DeclarationStatement");

    fmt::print(_out, "{: >{}}", "", getCurrentShift());
    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(LoopingStatement *expr)
{
    PRINT("LoopingStatement");

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
    PRINT("CompoundStatement");

    for (auto item : expr->getBody())
    {
        item->print(this);
    }
}

void Printer::print(ConditionStatement *expr)
{
    PRINT("ConditionStatement");

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
    PRINT("FieldExpression");

    printAsOperand(expr);

    fmt::println(_out, "");
}

void Printer::print(IndexExpression *expr)
{
    PRINT("ArrayExpression");

    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(AssignmentExpression *expr)
{
    PRINT("AssignmentExpression");

    fmt::print(_out, "{: >{}}", "", getCurrentShift());
    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, " = ");
    expr->getRHS()->printAsOperand(this);
    fmt::println(_out, "");
}

void Printer::print(CallExpression *expr)
{
    PRINT("CallExpression");

    fmt::print(_out, "{: >{}}", "", getCurrentShift());
    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(ReturnExpression *expr)
{
    PRINT("ReturnExpression");

    fmt::print(_out, "{: >{}}", "", getCurrentShift());
    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(UnaryExpression *expr)
{
    PRINT("UnaryExpression");

    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(BoolLiteralExpression *expr)
{
    PRINT("BoolLiteralExpression");

    fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void Printer::print(NumberLiteralExpression *expr)
{
    PRINT("NumberLiteralExpression");

    fmt::println(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void Printer::print(StringLiteralExpression *expr)
{
    PRINT("StringLiteralExpression");

    auto val = expr->getValue();
    util::replaceInPlace(val, std::string("\n"), std::string("\\n"));
    fmt::print(_out, "\"{}\"", val);
}

void Printer::print(VariableExpression *expr)
{
    PRINT("VariableExpression") << " " << expr->getIdentifier();
    assert(expr->getType());

    fmt::print(_out, "{} {}", expr->getIdentifier(), expr->getType()->getTypeName());
}

void Printer::print(ComparisonExpression *expr)
{
    PRINT("ComparisonExpression");

    printAsOperand(expr);
    fmt::println(_out, "");
}

void Printer::print(StructExpression *expr)
{
    PRINT("StructExpression");

    printAsOperand(expr);
    fmt::println(_out, "");
}

//
// PRINT AS OPERAND //
// SIMPLE OPERAND WITHPUT NEWLINE INSTRUCTION //
//

void Printer::printAsOperand(MethodCallExpression *expr)
{
    PRINT_OPERAND("MethodCallExpression");

    fmt::print(_out, "(");
    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, ").{} {}", expr->getIdentifier(), expr->getType()->getTypeName());
}

void Printer::printAsOperand(FieldExpression *expr)
{
    PRINT_OPERAND("FieldExpression");

    if (typeid(VariableExpression) == typeid(*expr->getLHS().get()))
    {
        fmt::print(_out, "{}", static_pointer_cast<VariableExpression>(expr->getLHS())->getIdentifier());
    }
    else
    {
        fmt::print(_out, "(");
        expr->getLHS()->printAsOperand(this);
        fmt::print(_out, ")");
    }

    fmt::print(_out, ".{} {}", expr->getField(), expr->getType()->getTypeName());
}

void Printer::printAsOperand(BreakExpression *expr)
{
    PRINT_OPERAND("BreakExpression");

    fmt::print(_out, "{: >{}}break", "", getCurrentShift());
    if (expr->getValue())
    {
        fmt::print(_out, "(");
        expr->getValue()->printAsOperand(this);
        fmt::print(_out, ")");
    }
}

void Printer::printAsOperand(GlobalVariable *expr)
{
    PRINT_OPERAND("GlobalVariable");

    fmt::print(_out, "@define {} {} = ", expr->getIdentifier(), expr->getType()->getTypeName());
    if (expr->getValue())
    {
        expr->getValue()->printAsOperand(this);
    }
}

void Printer::printAsOperand(StructExpression *expr)
{
    PRINT_OPERAND("StructExpression");

    fmt::print(_out, "{} {{", expr->getIdentifier());
    for (auto &item : expr->getFields())
    {
        fmt::print(_out, "{}: ", item->getIdentifier());

        item->getValue()->printAsOperand(this);
        fmt::print(_out, ",");
    }

    fmt::print(_out, "}}");
}

void Printer::printAsOperand(NumberLiteralExpression *expr)
{
    PRINT_OPERAND("NumberLiteralExpression");

    fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void Printer::printAsOperand(DoubleLiteralExpression *expr)
{
    PRINT_OPERAND("DoubleLiteralExpression");

    fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void Printer::printAsOperand(BoolLiteralExpression *expr)
{
    PRINT_OPERAND("BoolLiteralExpression");

    fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void Printer::printAsOperand(VariableExpression *expr)
{
    PRINT_OPERAND("VariableExpression");

    fmt::print(_out, "{} {}", expr->getIdentifier(), expr->getType()->getTypeName());
}

void Printer::printAsOperand(AssignmentExpression *expr)
{
    PRINT_OPERAND("AssignmentExpression");

    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, " = ");
    expr->getRHS()->printAsOperand(this);
}

void Printer::printAsOperand(ComparisonExpression *expr)
{
    PRINT_OPERAND("ComparisonExpression");

    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, " {} ", expr->getOperator().getValue());
    expr->getRHS()->printAsOperand(this);
}

void Printer::printAsOperand(CallExpression *expr)
{
    PRINT_OPERAND("CallExpression");

    fmt::print(_out, "@call (");
    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, ") (");
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

void Printer::printAsOperand(IndexExpression *expr)
{
    PRINT_OPERAND("ArrayExpression");

    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, "[");
    expr->getIndex()->printAsOperand(this);
    fmt::print(_out, "]");
}

void Printer::printAsOperand(ReturnExpression *expr)
{
    PRINT_OPERAND("ReturnExpression");

    fmt::print(_out, "return");
    if (!expr->getType()->isVoidType())
    {
        fmt::print(_out, " ");
        expr->getValue()->print(this);
    }
}

void Printer::printAsOperand(UnaryExpression *expr)
{
    PRINT_OPERAND("UnaryExpression");

    assert(expr->getType());

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

    fmt::print(_out, "{}(", op);
    expr->getValue()->printAsOperand(this);
    fmt::print(_out, ") {}", expr->getType()->getTypeName());
}

void Printer::printAsOperand(DeclarationStatement *expr)
{
    PRINT_OPERAND("DeclarationStatement");

    auto prefix = "@declare";
    if (expr->getValue())
    {
        prefix = "@define";
    }

    fmt::print(_out, "{} {} {} {}", prefix, printAsOperand(expr->getQualifier()), expr->getIdentifier(), expr->getType()->getTypeName());
    if (expr->getValue())
    {
        fmt::print(_out, " = ");

        expr->getValue()->printAsOperand(this);
    }
}

void Printer::printAsOperand(ArithmeticExpression *expr)
{
    PRINT_OPERAND("ArithmeticExpression");

    expr->getLHS()->printAsOperand(this);
    fmt::print(_out, " {} ", expr->getOperator().getValue());
    expr->getRHS()->printAsOperand(this);
}

void Printer::printAsOperand(StringLiteralExpression *expr)
{
    PRINT_OPERAND("StringLiteralExpression");

    auto val = util::normalizeStringLiteral(expr->getValue());
    fmt::print(_out, "\"{}\"", val);
}

void Printer::printAsOperand(CharLiteralExpression *expr)
{
    PRINT_OPERAND("CharLiteralExpression");

    auto val = util::normalizeStringLiteral(std::string(1, expr->getValue()));
    fmt::print(_out, "\'{}\'", val);
}

void Printer::printAsOperand(FloatLiteralExpression *expr)
{
    PRINT_OPERAND("FloatLiteralExpression");

    fmt::print(_out, "{} {}", expr->getValue(), expr->getType()->getTypeName());
}

void Printer::printAsOperand(ArrayExpression *expr)
{
    PRINT_OPERAND("ArrayLiteralExpression");

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

void Printer::printAsOperand(TypeCastExpression *expr)
{
    PRINT_OPERAND("TypeCastExpression");

    expr->getValue()->printAsOperand(this);
    fmt::print(_out, " as {}", expr->getType()->getTypeName());
}

void Printer::printAsOperand(NilLiteralExpression *expr)
{
    PRINT_OPERAND("NilLiteralExpression");

    fmt::print(_out, "nil");
}
