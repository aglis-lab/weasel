#include <weasel/Type/Type.h>
#include <weasel/Analysis/AnalysisSemantic.h>

using namespace weasel;

void FunctionType::accept(AnalysisSemantic *context)
{
    return context->accept(this);
}

void StructType::accept(AnalysisSemantic *context)
{
    return context->accept(this);
}

void Type::accept(AnalysisSemantic *context)
{
    return context->accept(this);
}
