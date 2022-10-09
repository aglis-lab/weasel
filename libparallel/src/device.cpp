#include "parallel.h"

// TODO: More spesific and advance feature
// for a ‘real-world application' you’d want to find which device best suits your workload by using
// vkGetPhysicalDeviceFeatures, vkGetPhysicalDeviceFormatProperties,
// vkGetPhysicalDeviceImageFormatProperties, vkGetPhysicalDeviceProperties,
// vkGetPhysicalDeviceQueueFamilyProperties and vkGetPhysicalDeviceMemoryProperties.
std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance instance)
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    if (physicalDeviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> arr(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, arr.data());

    return arr;
}

std::vector<VkQueueFamilyProperties> getQueueFamilyPoperties(VkPhysicalDevice physicalDevice)
{
    uint32_t queuePropertyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queuePropertyCount, nullptr);

    std::vector<VkQueueFamilyProperties> arr(queuePropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queuePropertyCount, arr.data());

    return arr;
}

std::vector<char *> getDeviceExtensions()
{
    return {
        "VK_KHR_portability_subset",
    };
}

void __setupDevice()
{
    // Get Physical Devices
    __physicalDevices = getPhysicalDevices(__instance);
    __physicalDevice = __physicalDevices.front();

    // Get Device Properties
    // VkPhysicalDeviceProperties physicalDeviceProperties;
    // vkGetPhysicalDeviceProperties(__physicalDevice, &physicalDeviceProperties);
    // std::cout << "Properties of device\n";
    // std::cout << "Vendor ID : " << physicalDeviceProperties.vendorID << std::endl;
    // std::cout << "Device Name : " << physicalDeviceProperties.deviceName << std::endl;
    // std::cout << "Vulkan Version : " << (physicalDeviceProperties.apiVersion >> 22) << std::endl;
    // std::cout << "Max Compute Shared Memory Size: " << physicalDeviceProperties.limits.maxComputeSharedMemorySize / 1024 << " KB" << std::endl;

    auto queueProperties = getQueueFamilyPoperties(__physicalDevice);
    auto queuePropSize = queueProperties.size();
    for (size_t i = 0; i < queuePropSize; i++)
    {
        auto prop = queueProperties[i];
        if (prop.queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT)
        {
            __familyIndex = i;
            break;
        }
    }

    // TODO: I don't know what it is
    float queuePriority[1] = {0.0};

    VkDeviceQueueCreateInfo queueInfo;
    queueInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.flags = 0;
    queueInfo.queueFamilyIndex = __familyIndex;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queuePriority;
    queueInfo.pNext = nullptr;

    std::vector<char *> deviceExtensions = getDeviceExtensions();
    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledExtensionCount = deviceExtensions.size();
    deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = nullptr;
    deviceInfo.pEnabledFeatures = nullptr;
    deviceInfo.pNext = nullptr;

    if (vkCreateDevice(__physicalDevice, &deviceInfo, nullptr, &__device) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create device!");
    }
}
