#include "parallel.h"

VkCommandPool createCommandPool(uint32_t familyIndex)
{
    // Command Pool
    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = familyIndex;

    VkCommandPool commandPool{};
    if (vkCreateCommandPool(__device, &commandPoolCreateInfo, nullptr, &commandPool) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }

    return commandPool;
}

std::vector<VkCommandBuffer> createCommandBuffers(VkCommandPool commandPool, uint32_t commandBufferCount)
{
    // Command Buffer
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.commandBufferCount = commandBufferCount;

    std::vector<VkCommandBuffer> commandBuffers(commandBufferCount);
    if (vkAllocateCommandBuffers(__device, &commandBufferAllocateInfo, commandBuffers.data()) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    return commandBuffers;
}

void parseCommand(VkCommandBuffer commandBuffer, VkPipeline pipeline, VkPipelineLayout pipelineLayout, std::vector<VkDescriptorSet> descriptorSets, ParallelGroup group)
{
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    commandBufferBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin command buffer!");
    }

    vkCmdBindPipeline(commandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, descriptorSets.size(), descriptorSets.data(), 0, nullptr);
    vkCmdDispatch(commandBuffer, group.x, group.y, group.z);

    if (vkEndCommandBuffer(commandBuffer))
    {
        throw std::runtime_error("failed to end command buffer!");
    }
}

void queueSubmit(VkQueue queue, std::vector<VkCommandBuffer> commandBuffers, VkFence fence)
{
    VkSubmitInfo submitInfo;
    submitInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = commandBuffers.size();
    submitInfo.pCommandBuffers = commandBuffers.data();
    submitInfo.pNext = nullptr;

    if (vkQueueSubmit(queue, 1, &submitInfo, fence) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to queue submit!");
    }
}
