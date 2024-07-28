#include <weasel/Type/Type.h>
#include <weasel/Analysis/AnalysisSemantic.h>

using namespace weasel;

void StructType::semantic(AnalysisSemantic *context)
{
    return context->semantic(this);
}

void Type::semantic(AnalysisSemantic *context)
{
    return context->semantic(this);
}
