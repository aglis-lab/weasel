#include <weasel/Package/PackageManager.h>

PackageHandle PackageManager::createPackageApp()
{
    _app = make_shared<Package>();
    return _app;
}

PackageHandle PackageManager::getPackageApp()
{
    return _app;
}

PackageManager PackageManager::getInstance()
{
    static PackageManager instance;

    return instance;
}
