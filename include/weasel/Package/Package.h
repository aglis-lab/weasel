#pragma once

#include <vector>

#include <weasel/Package/Module.h>

using namespace std;

namespace weasel
{
    class Package;
    using PackageHandle = shared_ptr<Package>;

    class Package
    {
    public:
        explicit Package() {}

        void addModule(ModuleHandle newModule) { _modules.push_back(newModule); }

    private:
        vector<PackageHandle> _packages;
        vector<ModuleHandle> _modules;
    };
} // namespace weasel
