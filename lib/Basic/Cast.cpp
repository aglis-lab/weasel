#include "weasel/Basic/Cast.h"

using namespace weasel;

template <class T>
bool isDynCast(Expression *expr)
{
    return dynamic_cast<T>(expr) != nullptr;
}
