#include <weasel-c/debug.h>

void debug(const std::vector<weasel::Function *> objects)
{
    // LOG(INFO) << std::endl
    //           << std::setfill('=') << std::setw(40) << "=" << std::endl
    //           << std::endl;

    std::cerr << std::endl;

    for (auto &obj : objects)
    {
        obj->debug(0);
    }

    std::cerr << std::endl;

    // LOG(INFO) << std::endl
    //           << std::setfill('=') << std::setw(40) << "=" << std::endl
    //           << std::endl;
}
