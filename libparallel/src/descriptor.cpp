#include "parallel.h"

VkDescriptorPoolSize createDescriptorPoolSize(uint32_t descriptorCount)
{
    VkDescriptorPoolSize poolSize;
    poolSize.type = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize.descriptorCount = descriptorCount;

    return poolSize;
}

VkDescriptorPool createDescriptorPool(std::vector<VkDescriptorPoolSize> pools, uint32_t maxSets)
{
    VkDescriptorPoolCreateInfo poolCreateInfo;
    poolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.flags = VkDescriptorPoolCreateFlagBits::VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    poolCreateInfo.poolSizeCount = pools.size();
    poolCreateInfo.pPoolSizes = pools.data();
    poolCreateInfo.maxSets = maxSets;
    poolCreateInfo.pNext = nullptr;

    VkDescriptorPool pool{};
    if (vkCreateDescriptorPool(__device, &poolCreateInfo, nullptr, &pool) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    return pool;
}

std::vector<VkDescriptorSet> allocateDescriptorSets(VkDescriptorPool pool, std::vector<VkDescriptorSetLayout> layouts)
{
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorSetCount = layouts.size();
    allocateInfo.pSetLayouts = layouts.data();
    allocateInfo.descriptorPool = pool;
    allocateInfo.pNext = nullptr;

    std::vector<VkDescriptorSet> descriptorSets(layouts.size());
    if (vkAllocateDescriptorSets(__device, &allocateInfo, descriptorSets.data()) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    return descriptorSets;
}

VkWriteDescriptorSet createDescriptorSet(VkDescriptorSet descriptorSet, size_t binding, VkBuffer buffer, size_t bufferSize)
{
    auto descriptorBufferInfo = new VkDescriptorBufferInfo;
    descriptorBufferInfo->buffer = buffer;
    descriptorBufferInfo->range = bufferSize;
    descriptorBufferInfo->offset = 0;

    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writeDescriptorSet.pBufferInfo = descriptorBufferInfo;
    writeDescriptorSet.dstBinding = binding;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.dstSet = descriptorSet;

    return writeDescriptorSet;
}

void updateDescriptorSets(std::vector<VkWriteDescriptorSet> descriptorWrites, std::vector<VkCopyDescriptorSet> descriptorCopies)
{
    vkUpdateDescriptorSets(__device, descriptorWrites.size(), descriptorWrites.data(), descriptorCopies.size(), descriptorCopies.data());
}
