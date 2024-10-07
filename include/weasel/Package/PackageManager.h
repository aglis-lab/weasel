#pragma once

#include <weasel/Package/Package.h>

namespace weasel
{
    // TODO:
    // Load Library
    // Get Libraries Source
    // Delete Library
    class PackageManager
    {
    private:
        PackageList _libraries;
        PackageHandle _app;

        PackageManager() {}

    public:
        static PackageManager getInstance();

        PackageHandle createPackageApp();
        PackageHandle getPackageApp();
    };
} // namespace weasel
