#include "parallel.h"

VkBuffer createBuffer(size_t bufferSize)
{
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.size = bufferSize;
    bufferCreateInfo.pQueueFamilyIndices = &__familyIndex;
    bufferCreateInfo.queueFamilyIndexCount = 1;

    VkBuffer buffer;
    if (vkCreateBuffer(__device, &bufferCreateInfo, nullptr, &buffer) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    return buffer;
}

VkDeviceMemory createDeviceMemory(VkBuffer buffer)
{
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(__device, buffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo;
    memoryAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = __memoryTypeIndex;
    memoryAllocateInfo.pNext = nullptr;

    VkDeviceMemory deviceMemory;
    if (vkAllocateMemory(__device, &memoryAllocateInfo, nullptr, &deviceMemory) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate memory!");
    }

    if (vkBindBufferMemory(__device, buffer, deviceMemory, 0) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to bind buffer memory!");
    }

    return deviceMemory;
}

BufferMemory *getAllocation(void *dataPtr)
{
    auto ptr = reinterpret_cast<std::uintptr_t>(dataPtr);
    auto isAllocated = __bufferMemory.find(ptr) != __bufferMemory.end();
    if (!isAllocated)
    {
        return nullptr;
    }

    return __bufferMemory[ptr];
}

void __deAllocation(void *dataPtr)
{
    auto ptr = reinterpret_cast<std::uintptr_t>(dataPtr);
    auto isAllocated = __bufferMemory.find(ptr) != __bufferMemory.end();
    if (!isAllocated)
    {
        return;
    }

    auto bufferMemory = __bufferMemory[ptr];

    vkUnmapMemory(__device, bufferMemory->deviceMemory);
    vkFreeMemory(__device, bufferMemory->deviceMemory, nullptr);
    vkDestroyBuffer(__device, bufferMemory->buffer, nullptr);
}

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

void *__allocate(void *dataPtr, uint32_t bufferSize)
{
    if (dataPtr != nullptr)
    {
        auto ptr = reinterpret_cast<std::uintptr_t>(dataPtr);
        if (__bufferMemory.find(ptr) != __bufferMemory.end())
        {
            auto val = __bufferMemory[ptr];

            vkUnmapMemory(__device, val->deviceMemory);
            vkFreeMemory(__device, val->deviceMemory, nullptr);
            vkDestroyBuffer(__device, val->buffer, nullptr);

            __bufferMemory.erase(ptr);
        }
    }

    auto buffer = createBuffer(bufferSize);
    auto deviceMemory = createDeviceMemory(buffer);

    // Map to device memory
    vkMapMemory(__device, deviceMemory, 0, bufferSize, 0, (void **)&dataPtr);

    auto ptr = reinterpret_cast<std::uintptr_t>(dataPtr);
    auto mem = new BufferMemory;
    mem->bufferSize = bufferSize;
    mem->buffer = buffer;
    mem->deviceMemory = deviceMemory;

    __bufferMemory[ptr] = mem;

    return dataPtr;
}
