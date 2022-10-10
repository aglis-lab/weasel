#include "helper.h"
#include "parallel.h"

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

    // Create Descriptor Pool
    auto poolSize = createDescriptorPoolSize(descriptorCount);
    auto poolsSize = std::vector<VkDescriptorPoolSize>{poolSize};
    auto pool = createDescriptorPool(poolsSize, layouts.size());

    // Allocate Info Descriptor Sets
    auto descriptorSets = allocateDescriptorSets(pool, layouts);
    auto descriptorSet = descriptorSets.front();

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

    updateDescriptorSets(writeDescriptorSets, {});

    // Create Pipeline Layout
    // Pipeline layout for all the functions parameters
    // Multiple Layout to set layout on GLSL
    auto pipelineLayout = createPipelineLayout(layouts);

    // Create Pipeline
    // We can create multiple function and multiple pipelines at once
    auto computeCreateInfo = createComputePipelineCreateInfo(functionName, pipelineLayout);
    auto pipelineCache = createPipelineCache();
    auto pipelines = createPipelines({computeCreateInfo}, pipelineCache);
    auto pipeline = pipelines[0];

    // Command Pool
    auto commandPool = createCommandPool(__familyIndex);

    // Command Buffer
    auto commandBufferCount = (uint)1;
    auto commandBuffers = createCommandBuffers(commandPool, commandBufferCount);
    auto commandBuffer = commandBuffers.front();

    // Recording Command
    parseCommand(commandBuffer, pipeline, pipelineLayout, descriptorSets, group);

    // Queue and Fence
    auto fence = createFence();

    // Create Device Queue
    auto queue = getDeviceQueue(__familyIndex, 0);

    // Run GPU
    queueSubmit(queue, commandBuffers, fence);

    // Wait for Queue Finish
    waitFences({fence}, true, -1);

    // // Second Attempt
    // {
    //     std::vector<VkWriteDescriptorSet> writeDescriptorSets(paramCount);
    //     for (size_t i = 0; i < paramCount; i++)
    //     {
    //         auto arg = args[i];
    //         auto bufferMemory = arg.bufferMemory;
    //         assert(bufferMemory);

    //         auto descriptor = createDescriptorSet(descriptorSet, i, bufferMemory->buffer, bufferMemory->bufferSize);
    //         writeDescriptorSets[i] = descriptor;
    //     }

    //     updateDescriptorSets(writeDescriptorSets, {});
    //     parseCommand(commandBuffer, pipeline, pipelineLayout, descriptorSets, group);
    //     resetFences({fence});
    // }

    // for (size_t i = 0; i < 10; i++)
    // {
    //     ((float *)args[0].data)[i] *= 2;
    // }

    // // Run GPU
    // auto fence2 = createFence();
    // queueSubmit(queue, commandBuffers, fence2);

    // // Wait for Queue Finish
    // waitFences({fence2, fence}, true, -1);

    for (auto item : args)
    {
        if (item.modifier == Modifier::TEMPORARY)
        {
            __deAllocation(item.data);
        }
    }
}
