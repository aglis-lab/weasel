#include <string>

#include "weasel/Passes/PassesGLSL.h"

bool weasel::PassesGLSL::run()
{
    std::string glsl = "#version 430 core\n";
    auto currentFun = _functions[0];

    currentFun->getArguments();

    return true;
}