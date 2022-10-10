#include "parallel.h"

VkFence createFence()
{
    VkFence fence;
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = 0;

    if (vkCreateFence(__device, &fenceCreateInfo, nullptr, &fence) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create fence!");
    }

    return fence;
}

VkQueue getDeviceQueue(uint familyIndex, uint queueIndex)
{
    VkQueue queue;
    vkGetDeviceQueue(__device, familyIndex, queueIndex, &queue);

    return queue;
}

void waitFences(std::vector<VkFence> fences, bool waitAll, uint timeout)
{
    if (vkWaitForFences(__device, fences.size(), fences.data(), waitAll, timeout) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to wait for fence!");
    }
}

void resetFences(std::vector<VkFence> fences)
{
    if (vkResetFences(__device, fences.size(), fences.data()) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to reset fences!");
    }
}

VkResult getFenceStatus(VkFence fence)
{
    return vkGetFenceStatus(__device, fence);
}
