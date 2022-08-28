#include <vulkan/vulkan.hpp>

#include <vector>
#include <iostream>
#include <fstream>

// for a ‘real-world application' you’d want to find which device best suits your workload by using
// vkGetPhysicalDeviceFeatures, vkGetPhysicalDeviceFormatProperties,
// vkGetPhysicalDeviceImageFormatProperties, vkGetPhysicalDeviceProperties,
// vkGetPhysicalDeviceQueueFamilyProperties and vkGetPhysicalDeviceMemoryProperties.
std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance instance)
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    std::vector<VkPhysicalDevice> arr(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, arr.data());

    return arr;
}

std::vector<VkQueueFamilyProperties> getFamilyProperties(VkPhysicalDevice device)
{
    uint32_t propertyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &propertyCount, nullptr);

    std::vector<VkQueueFamilyProperties> arr(propertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &propertyCount, arr.data());

    return arr;
}

std::vector<VkQueueFamilyProperties> getQueueFamilyPoperties(VkPhysicalDevice physicalDevice)
{
    uint32_t queuePropertyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queuePropertyCount, nullptr);

    std::vector<VkQueueFamilyProperties> arr(queuePropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queuePropertyCount, arr.data());

    return arr;
}

std::vector<char *> getDeviceExtensions()
{
    return {
        "VK_KHR_portability_subset"};
}

std::vector<VkExtensionProperties> getExtensionProperties()
{
    uint32_t propertyCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr);

    std::vector<VkExtensionProperties> properties(propertyCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, properties.data());

    return properties;
}

std::vector<char *> getMinimumExtensions()
{
    return {
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    };
}

std::vector<VkLayerProperties> getLayerProperties()
{
    uint32_t count;
    vkEnumerateInstanceLayerProperties(&count, nullptr);

    std::vector<VkLayerProperties> props(count);
    vkEnumerateInstanceLayerProperties(&count, props.data());

    return props;
}

std::vector<uint32_t> readBinaryFile(const char *filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "File not openned\n";
        exit(1);
    }

    std::vector<char> spirv((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

    file.close();

    // Copy data from the char-vector to a new uint32_t-vector
    std::vector<uint32_t> spv(spirv.size());

    memcpy(spv.data(), spirv.data(), spirv.size());

    return spv;
}

int main()
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

    // Get Instance
    VkApplicationInfo appInfo;
    appInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Testing Vulkan Compute";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Testing Vulkan Compute Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    appInfo.pNext = nullptr;

    VkInstanceCreateInfo createInfo;
    createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.flags = VkInstanceCreateFlagBits::VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = 0;
    // createInfo.enabledLayerCount = layers.size();
    // createInfo.ppEnabledLayerNames = layers.data();
    createInfo.pNext = nullptr;

    // Get Instance
    VkInstance instance;
    VkResult res = vkCreateInstance(&createInfo, nullptr, &instance);
    if (res != VkResult::VK_SUCCESS)
    {
        std::cout << "failed to create instance with error_code " << res << std::endl;
        throw std::runtime_error("failed to create instance!");
    }

    // Get Physical Devices
    auto physicalDevices = getPhysicalDevices(instance);
    auto physicalDevice = physicalDevices.front();
    for (auto item : physicalDevices)
    {
        std::cout << "Device : " << item << std::endl;
    }

    // Get Device Properties
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    std::cout << "Properties of device\n";
    std::cout << "Vendor ID : " << physicalDeviceProperties.vendorID << std::endl;
    std::cout << "Device Name : " << physicalDeviceProperties.deviceName << std::endl;
    std::cout << "Vulkan Version : " << (physicalDeviceProperties.apiVersion >> 22) << std::endl;
    std::cout << "Max Compute Shared Memory Size: " << physicalDeviceProperties.limits.maxComputeSharedMemorySize / 1024 << " KB" << std::endl;

    // Get Queue Family Properties
    std::vector<VkQueueFamilyProperties> queueProperties = getQueueFamilyPoperties(physicalDevice);

    uint32_t familyIndex = -1;
    uint32_t queuePropSize = queueProperties.size();
    for (uint32_t i = 0; i < queuePropSize; i++)
    {
        auto prop = queueProperties[i];
        if (prop.queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT)
        {
            familyIndex = i;
            break;
        }
    }

    std::cout << "Compute Queue Family Index " << familyIndex << " Of " << queuePropSize << std::endl;

    // Create Device
    float queuePriority[1] = {0.0};

    VkDeviceQueueCreateInfo queueInfo;
    queueInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = familyIndex;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queuePriority;
    queueInfo.pNext = nullptr;

    std::vector<char *> deviceExtensions = getDeviceExtensions();
    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledExtensionCount = deviceExtensions.size();
    deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = nullptr;
    deviceInfo.pEnabledFeatures = nullptr;
    deviceInfo.pNext = nullptr;

    VkDevice device;
    vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device);

    // Elements
    uint32_t numElements = 10;
    uint32_t bufferSize = numElements * sizeof(int32_t);

    // Creating Buffer //
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferCreateInfo.sharingMode = VkSharingMode::VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.size = bufferSize;
    bufferCreateInfo.pQueueFamilyIndices = &familyIndex;
    bufferCreateInfo.queueFamilyIndexCount = 1;

    VkBuffer inBuffer;
    VkBuffer outBuffer;

    vkCreateBuffer(device, &bufferCreateInfo, nullptr, &inBuffer);
    vkCreateBuffer(device, &bufferCreateInfo, nullptr, &outBuffer);

    VkMemoryRequirements inMemoryRequirements;
    VkMemoryRequirements outMemoryRequirements;

    vkGetBufferMemoryRequirements(device, inBuffer, &inMemoryRequirements);
    vkGetBufferMemoryRequirements(device, outBuffer, &outMemoryRequirements);

    // Physical Device Memory Properties
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    uint32_t memoryTypeIndex = 0;
    std::cout << std::endl;

    auto memoryTypeCount = memoryProperties.memoryTypeCount;
    for (uint32_t i = 0; i < memoryTypeCount; i++)
    {
        auto memoryType = memoryProperties.memoryTypes[i];

        if ((memoryType.propertyFlags & VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) &&
            (memoryType.propertyFlags & VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
        {
            memoryTypeIndex = i;
        }

        std::bitset<16> val(memoryType.propertyFlags);
        std::cout << "Memory Type " << val << std::endl;
    }
    std::cout << std::endl;

    uint32_t memoryHeapCount = memoryProperties.memoryHeapCount;
    for (uint32_t i = 0; i < memoryHeapCount; i++)
    {
        auto memoryType = memoryProperties.memoryHeaps[i];

        std::bitset<16> val(memoryType.flags);
        std::cout << "Memory Heap " << val << " : " << memoryType.size / 1024 / 1024 / 1024 << " GB" << std::endl;
    }

    // Checking Memory Property
    std::cout << std::endl;
    std::cout << "Memory Host Visible " << std::bitset<16>(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) << std::endl;
    std::cout << "Memory Host Coherent " << std::bitset<16>(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) << std::endl;
    std::cout << "Memory Host Cached " << std::bitset<16>(VK_MEMORY_PROPERTY_HOST_CACHED_BIT) << std::endl;
    std::cout << "Memory Device Local " << std::bitset<16>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) << std::endl;
    std::cout << "Memory Device Coherent " << std::bitset<16>(VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD) << std::endl;
    std::cout << "Memory Type Index " << memoryTypeIndex << std::endl;

    // Load Content
    char *filePath = "/Users/zaen/Projects/Open Source/weasel/test/vulkan-test/square.spv";
    auto contentProgram = readBinaryFile(filePath);

    // Create Shader Module
    VkShaderModuleCreateInfo shaderCreateInfo{};
    shaderCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreateInfo.codeSize = contentProgram.size();
    shaderCreateInfo.pCode = contentProgram.data();

    VkShaderModule shaderModule{};
    vkCreateShaderModule(device, &shaderCreateInfo, nullptr, &shaderModule);

    // Set Descriptor Layout
    VkDescriptorSetLayoutBinding setLayoutBindingFirst;
    setLayoutBindingFirst.binding = 0;
    setLayoutBindingFirst.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    setLayoutBindingFirst.descriptorCount = 1;
    setLayoutBindingFirst.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
    setLayoutBindingFirst.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding setLayoutBindingSecond;
    setLayoutBindingSecond.binding = 1;
    setLayoutBindingSecond.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    setLayoutBindingSecond.descriptorCount = 1;
    setLayoutBindingSecond.stageFlags = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
    setLayoutBindingSecond.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> layoutBinding = {setLayoutBindingFirst, setLayoutBindingSecond};

    VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
    layoutCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.pBindings = layoutBinding.data();
    layoutCreateInfo.bindingCount = layoutBinding.size();

    VkDescriptorSetLayout layout{};
    vkCreateDescriptorSetLayout(device, &layoutCreateInfo, nullptr, &layout);

    // Create Pipeline Layout
    VkPipelineLayoutCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineCreateInfo.pSetLayouts = &layout;
    pipelineCreateInfo.setLayoutCount = 1;

    VkPipelineLayout pipeline{};
    vkCreatePipelineLayout(device, &pipelineCreateInfo, nullptr, &pipeline);

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
    pipelineCacheCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

    VkPipelineCache pipelineCache{};
    vkCreatePipelineCache(device, &pipelineCacheCreateInfo, nullptr, &pipelineCache);

    // Create Pipeline
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo{};
    shaderStageCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageCreateInfo.module = shaderModule;
    shaderStageCreateInfo.pName = "main";

    VkComputePipelineCreateInfo computeCreateInfo{};
    computeCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computeCreateInfo.layout = pipeline;
    computeCreateInfo.stage = shaderStageCreateInfo;

    VkPipeline computePipeline{};
    vkCreateComputePipelines(device, pipelineCache, 1, &computeCreateInfo, nullptr, &computePipeline);

    // Create Descriptor Pool
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize.descriptorCount = 2;

    VkDescriptorPoolCreateInfo poolCreateInfo;
    poolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolCreateInfo.flags = VkDescriptorPoolCreateFlagBits::VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    poolCreateInfo.pPoolSizes = &poolSize;
    poolCreateInfo.poolSizeCount = 1;
    poolCreateInfo.pNext = nullptr;

    VkDescriptorPool pool{};
    vkCreateDescriptorPool(device, &poolCreateInfo, nullptr, &pool);

    // Allocate Info
    VkDescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.pSetLayouts = &layout;
    allocateInfo.descriptorPool = pool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pNext = nullptr;

    // Descriptor Sets
    std::vector<VkDescriptorSet> descriptorSets(allocateInfo.descriptorSetCount);
    vkAllocateDescriptorSets(device, &allocateInfo, descriptorSets.data());
    VkDescriptorSet descriptorSet = descriptorSets.front();

    // Descriptor Buffer
    VkDescriptorBufferInfo inDescriptorBufferInfo{};
    inDescriptorBufferInfo.buffer = inBuffer;
    inDescriptorBufferInfo.range = bufferSize;
    inDescriptorBufferInfo.offset = 0;

    VkDescriptorBufferInfo outDescriptorBufferInfo{};
    outDescriptorBufferInfo.buffer = outBuffer;
    outDescriptorBufferInfo.range = bufferSize;
    outDescriptorBufferInfo.offset = 0;

    // Write Descriptor Set
    VkWriteDescriptorSet inWriteDescriptorSet{};
    inWriteDescriptorSet.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    inWriteDescriptorSet.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    inWriteDescriptorSet.pBufferInfo = &inDescriptorBufferInfo;
    inWriteDescriptorSet.dstBinding = 0;
    inWriteDescriptorSet.dstArrayElement = 0;
    inWriteDescriptorSet.descriptorCount = 1;
    inWriteDescriptorSet.dstSet = descriptorSet;

    VkWriteDescriptorSet outWriteDescriptorSet{};
    outWriteDescriptorSet.sType = VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    outWriteDescriptorSet.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    outWriteDescriptorSet.pBufferInfo = &outDescriptorBufferInfo;
    outWriteDescriptorSet.dstBinding = 1;
    outWriteDescriptorSet.dstArrayElement = 0;
    outWriteDescriptorSet.descriptorCount = 1;
    outWriteDescriptorSet.dstSet = descriptorSet;

    std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
        inWriteDescriptorSet,
        outWriteDescriptorSet};

    vkUpdateDescriptorSets(device, writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);

    // Submitting work to GPU //
    // Command Pool
    VkCommandPoolCreateInfo commandPoolCreateInfo{};
    commandPoolCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex = familyIndex;

    VkCommandPool commandPool{};
    vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool);

    // Command Buffer
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    std::vector<VkCommandBuffer> commandBuffers(commandBufferAllocateInfo.commandBufferCount);
    vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers.data());
    VkCommandBuffer commandBuffer = commandBuffers.front();

    // Recording Command
    VkCommandBufferBeginInfo commandBufferBeginInfo{};
    commandBufferBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    commandBufferBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
    vkCmdBindPipeline(commandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
    vkCmdBindDescriptorSets(commandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pipeline, 0, 1, descriptorSets.data(), 0, nullptr);
    vkCmdDispatch(commandBuffer, numElements, 1, 1);
    vkEndCommandBuffer(commandBuffer);

    // Submit Work to GPU
    VkQueue queue;
    vkGetDeviceQueue(device, familyIndex, 0, &queue);

    VkFence fence;
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);

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

    // Device Memory
    VkMemoryAllocateInfo inMemoryAllocateInfo;
    inMemoryAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    inMemoryAllocateInfo.allocationSize = inMemoryRequirements.size;
    inMemoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
    inMemoryAllocateInfo.pNext = nullptr;

    VkMemoryAllocateInfo outMemoryAllocateInfo;
    outMemoryAllocateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    outMemoryAllocateInfo.allocationSize = outMemoryRequirements.size;
    outMemoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
    outMemoryAllocateInfo.pNext = nullptr;

    VkDeviceMemory inDeviceMemory;
    VkDeviceMemory outDeviceMemory;

    vkAllocateMemory(device, &inMemoryAllocateInfo, nullptr, &inDeviceMemory);
    vkAllocateMemory(device, &outMemoryAllocateInfo, nullptr, &outDeviceMemory);

    // Binding Memory
    vkBindBufferMemory(device, inBuffer, inDeviceMemory, 0);
    vkBindBufferMemory(device, outBuffer, outDeviceMemory, 0);

    // Map Input before submit
    uint32_t *inBufferPtr = (uint32_t *)malloc(bufferSize);
    vkMapMemory(device, inDeviceMemory, 0, bufferSize, 0, (void **)&inBufferPtr);

    for (uint32_t i = 0; i < numElements; i++)
    {
        inBufferPtr[i] = i + 1;
    }

    // Run GPU
    vkQueueSubmit(queue, 1, &submitInfo, fence);
    vkWaitForFences(device, 1, &fence, true, -1);

    // Map device memory to host
    uint32_t *outBufferPtr = (uint32_t *)malloc(bufferSize);
    vkMapMemory(device, outDeviceMemory, 0, bufferSize, 0, (void **)&outBufferPtr);

    std::cout << "In Values :\n";
    for (uint32_t i = 0; i < numElements; ++i)
    {
        std::cout << inBufferPtr[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Out Values :\n";
    for (uint32_t i = 0; i < numElements; ++i)
    {
        std::cout << outBufferPtr[i] << " ";
    }
    std::cout << std::endl;

    // Unmap memory
    vkUnmapMemory(device, outDeviceMemory);
    vkUnmapMemory(device, inDeviceMemory);

    // Release Everything
    vkResetCommandPool(device, commandPool, VkCommandPoolResetFlagBits::VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
    vkDestroyFence(device, fence, nullptr);
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
    vkDestroyPipelineLayout(device, pipeline, nullptr);
    vkDestroyPipelineCache(device, pipelineCache, nullptr);
    vkDestroyShaderModule(device, shaderModule, nullptr);
    vkDestroyPipeline(device, computePipeline, nullptr);
    vkDestroyDescriptorPool(device, pool, nullptr);
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkFreeMemory(device, inDeviceMemory, nullptr);
    vkFreeMemory(device, outDeviceMemory, nullptr);
    vkDestroyBuffer(device, inBuffer, nullptr);
    vkDestroyBuffer(device, outBuffer, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
}
