#include <iostream>

#include "parallel.h"

std::vector<VkExtensionProperties> getExtensionProperties()
{
    uint32_t propertyCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr);

    std::vector<VkExtensionProperties> properties(propertyCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, properties.data());

    return properties;
}

std::vector<VkLayerProperties> getLayerProperties()
{
    uint32_t count;
    vkEnumerateInstanceLayerProperties(&count, nullptr);

    std::vector<VkLayerProperties> props(count);
    vkEnumerateInstanceLayerProperties(&count, props.data());

    return props;
}

void __setupInstance()
{
    auto extensionProps = getExtensionProperties();
    std::vector<char *> extensions(extensionProps.size());
    for (uint32_t i = 0; i < extensionProps.size(); i++)
    {
        extensions[i] = extensionProps[i].extensionName;
    }

    auto layersProps = getLayerProperties();
    std::vector<char *> layers(layersProps.size());
    for (uint32_t i = 0; i < layersProps.size(); i++)
    {
        layers[i] = layersProps[i].layerName;
    }

    VkApplicationInfo appInfo;
    appInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Compute App on weasel language";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Vulkan Compute Engine on Weasel Language";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    appInfo.pNext = nullptr;

    VkInstanceCreateInfo createInfo;
    createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.flags = VkInstanceCreateFlagBits::VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = layers.size();
    createInfo.ppEnabledLayerNames = layers.data();
    createInfo.pNext = nullptr;

    VkResult res = vkCreateInstance(&createInfo, nullptr, &__instance);
    if (res != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
}

void __release()
{
    std::cout << "Release Vulkan Parallel\n";
}
