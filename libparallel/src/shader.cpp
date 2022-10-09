#include "helper.h"
#include "parallel.h"

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

void __shader(const char *filename)
{
    auto program = readBinaryFile(filename);

    // Create Shader Module
    VkShaderModuleCreateInfo shaderCreateInfo{};
    shaderCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.codeSize = program.size();
    shaderCreateInfo.pCode = program.data();

    if (vkCreateShaderModule(__device, &shaderCreateInfo, nullptr, &__shaderModule) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }
}

void __run(const char *functionName, ParallelGroup group, uint32_t paramCount, ...)
{
    // Descriptor Count should be equal with binding count
    auto bindingCount = paramCount;
    auto descriptorCount = bindingCount;
    std::vector<Argument> args;

    va_list vl;
    va_start(vl, paramCount);

    for (size_t i = 0; i < paramCount; i++)
    {
        auto val = va_arg(vl, Floats *);
        assert(val->size > 0);

        auto data = (void *)val->data;
        auto modifier = Modifier::PERSISTANT;
        auto allocated = getAllocation(data);

        auto arg = Argument{};
        if (allocated == nullptr)
        {
            auto bufferSize = val->size * sizeof(float);
            auto tempPtr = __allocate(nullptr, bufferSize);

            memcpy(tempPtr, data, bufferSize);

            allocated = getAllocation(tempPtr);
            modifier = Modifier::TEMPORARY;
            arg.data = tempPtr;
        }
        else
        {
            arg.data = data;
        }

        arg.modifier = modifier;
        arg.bufferMemory = allocated;

        args.push_back(arg);
    }

    va_end(vl);

    // Create Descriptor Set Layout
    // Set binding for each parameters
    auto layout = createDescriptorSetLayout(bindingCount);
    auto layouts = std::vector<VkDescriptorSetLayout>{layout};

    // Create Pipeline Layout
    // Pipeline layout for all the functions parameters
    // Multiple Layout to set layout on GLSL
    auto pipeline = createPipelineLayout(layouts);

    // Create Pipeline
    // We can create multiple function and multiple pipelines at once
    auto computeCreateInfo = createComputePipelineCreateInfo(functionName, pipeline);
    auto pipelineCache = createPipelineCache();
    auto pipelines = createPipelines({computeCreateInfo}, pipelineCache);
    auto computePipeline = pipelines[0];

    // Create Descriptor Pool
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize.descriptorCount = descriptorCount;

    VkDescriptorPoolCreateInfo poolCreateInfo;
    poolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.flags = VkDescriptorPoolCreateFlagBits::VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    poolCreateInfo.poolSizeCount = 1;
    poolCreateInfo.pPoolSizes = &poolSize;
    poolCreateInfo.maxSets = 1;
    poolCreateInfo.pNext = nullptr;

    VkDescriptorPool pool{};
    if (vkCreateDescriptorPool(__device, &poolCreateInfo, nullptr, &pool) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    // Allocate Info Descriptor Sets
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &layout;
    allocateInfo.descriptorPool = pool;
    allocateInfo.pNext = nullptr;

    std::vector<VkDescriptorSet> descriptorSets(allocateInfo.descriptorSetCount);
    if (vkAllocateDescriptorSets(__device, &allocateInfo, descriptorSets.data()) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
    VkDescriptorSet descriptorSet = descriptorSets.front();

    // Update Descriptor Set
    std::vector<VkWriteDescriptorSet> writeDescriptorSets(paramCount);
    for (size_t i = 0; i < paramCount; i++)
    {
        auto arg = args[i];
        auto bufferMemory = arg.bufferMemory;
        assert(bufferMemory);

        auto descriptor = createDescriptorSet(descriptorSet, i, bufferMemory->buffer, bufferMemory->bufferSize);
        writeDescriptorSets[i] = descriptor;
    }

    vkUpdateDescriptorSets(__device, writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);

    // Command Pool
    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = __familyIndex;

    VkCommandPool commandPool{};
    if (vkCreateCommandPool(__device, &commandPoolCreateInfo, nullptr, &commandPool) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }

    // Command Buffer
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    std::vector<VkCommandBuffer> commandBuffers(commandBufferAllocateInfo.commandBufferCount);
    if (vkAllocateCommandBuffers(__device, &commandBufferAllocateInfo, commandBuffers.data()) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    VkCommandBuffer commandBuffer = commandBuffers.front();

    // Recording Command
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    commandBufferBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
    vkCmdBindPipeline(commandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
    vkCmdBindDescriptorSets(commandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pipeline, 0, 1, descriptorSets.data(), 0, nullptr);
    vkCmdDispatch(commandBuffer, group.x, group.y, group.z);
    vkEndCommandBuffer(commandBuffer);

    // Submit Work to GPU
    VkFence fence;
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    if (vkCreateFence(__device, &fenceCreateInfo, nullptr, &fence) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create fence!");
    }

    // Create Device Queue
    VkQueue queue;
    vkGetDeviceQueue(__device, __familyIndex, 0, &queue);

    // Run GPU
    VkSubmitInfo submitInfo;
    submitInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.pNext = nullptr;

    if (vkQueueSubmit(queue, 1, &submitInfo, fence) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to queue submit!");
    }

    // Wait for Queue Finish
    if (vkWaitForFences(__device, 1, &fence, true, -1) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to wait for fence!");
    }

    for (auto item : args)
    {
        if (item.modifier == Modifier::TEMPORARY)
        {
            __deAllocation(item.data);
        }
    }
}
