#include <weasel/Type/Type.h>
#include <weasel/Analysis/AnalysisSemantic.h>

using namespace weasel;

void FunctionType::accept(AnalysisSemantic *context)
{
    return context->semantic(this);
}

void StructType::accept(AnalysisSemantic *context)
{
    return context->semantic(this);
}

void Type::accept(AnalysisSemantic *context)
{
    return context->semantic(this);
}
