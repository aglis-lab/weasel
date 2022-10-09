#include "parallel.h"

VkComputePipelineCreateInfo createComputePipelineCreateInfo(const char *funName, VkPipelineLayout pipelineLayout)
{
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo{};
    shaderStageCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageCreateInfo.module = __shaderModule;
    shaderStageCreateInfo.pName = funName;

    VkComputePipelineCreateInfo computeCreateInfo{};
    computeCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computeCreateInfo.layout = pipelineLayout;
    computeCreateInfo.stage = shaderStageCreateInfo;

    return computeCreateInfo;
}

std::vector<VkPipeline> createPipelines(std::vector<VkComputePipelineCreateInfo> createInfos, VkPipelineCache pipelineCache)
{
    std::vector<VkPipeline> pipelines(createInfos.size());
    if (vkCreateComputePipelines(__device, pipelineCache, createInfos.size(), createInfos.data(), nullptr, pipelines.data()) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create compute pipelines!");
    }

    return pipelines;
}

VkPipelineCache createPipelineCache()
{
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
    pipelineCacheCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCacheCreateInfo.flags = VkPipelineCacheCreateFlagBits::VK_PIPELINE_CACHE_CREATE_EXTERNALLY_SYNCHRONIZED_BIT;

    VkPipelineCache pipelineCache{};
    if (vkCreatePipelineCache(__device, &pipelineCacheCreateInfo, nullptr, &pipelineCache) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline cache!");
    }

    return pipelineCache;
}
