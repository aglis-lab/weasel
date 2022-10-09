#include "parallel.h"

VkDescriptorSetLayoutBinding createLayoutBinding(uint32_t binding)
{
    VkDescriptorSetLayoutBinding setLayoutBindingFirst;
    setLayoutBindingFirst.binding = binding;
    setLayoutBindingFirst.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    setLayoutBindingFirst.descriptorCount = 1;
    setLayoutBindingFirst.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
    setLayoutBindingFirst.pImmutableSamplers = nullptr;

    return setLayoutBindingFirst;
}

VkDescriptorSetLayout createDescriptorSetLayout(uint32_t bindingCount)
{
    std::vector<VkDescriptorSetLayoutBinding> layoutBinding;
    for (size_t i = 0; i < bindingCount; i++)
    {
        layoutBinding.push_back(createLayoutBinding(i));
    }

    VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
    layoutCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.pBindings = layoutBinding.data();
    layoutCreateInfo.bindingCount = layoutBinding.size();

    VkDescriptorSetLayout layout{};
    if (vkCreateDescriptorSetLayout(__device, &layoutCreateInfo, nullptr, &layout) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    return layout;
}

VkPipelineLayout createPipelineLayout(std::vector<VkDescriptorSetLayout> layouts)
{
    VkPipelineLayoutCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineCreateInfo.pSetLayouts = layouts.data();
    pipelineCreateInfo.setLayoutCount = layouts.size();

    VkPipelineLayout pipeline{};
    if (vkCreatePipelineLayout(__device, &pipelineCreateInfo, nullptr, &pipeline) != VkResult::VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    return pipeline;
}
