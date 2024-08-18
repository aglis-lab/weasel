#include <weasel/Type/Type.h>
#include <weasel/Analysis/AnalysisSemantic.h>

using namespace weasel;

#define UNIMPLEMENTED LOG(INFO) << "UNIMPLEMENTED; Type Error Analysis"

void FunctionType::accept(AnalysisError *context)
{
    UNIMPLEMENTED;
}

void ArrayType::accept(AnalysisError *context)
{
    UNIMPLEMENTED;
}

void StructType::accept(AnalysisError *context)
{
    UNIMPLEMENTED;
}

void Type::accept(AnalysisError *context)
{
    UNIMPLEMENTED;
}
