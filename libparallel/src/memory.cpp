#include "parallel.h"

void __setupMemory()
{
    // Physical Device Memory Properties
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(__physicalDevice, &memoryProperties);

    auto memoryTypeCount = memoryProperties.memoryTypeCount;
    for (uint32_t i = 0; i < memoryTypeCount; i++)
    {
        auto memoryType = memoryProperties.memoryTypes[i];

        if ((memoryType.propertyFlags & VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) &&
            (memoryType.propertyFlags & VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            __memoryTypeIndex = i;
        }
    }
}
