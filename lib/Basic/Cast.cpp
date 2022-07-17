#include "weasel/Basic/Cast.h"

template <class T>
bool weasel::isDynCast(Expression *expr)
{
    return dynamic_cast<T>(expr) != nullptr;
}
