#include <vulkan/vulkan.hpp>

#include <vector>
#include <iostream>
#include <unordered_map>

// CUDA REFERENCE
// cudaMalloc(void **devPtr, size_t count);
// cudaFree(void *devPtr);
// cudaMemcpy(void *dst, void *src, size_t count, cudaMemcpyKind kind)

// void main(){
//     float *a, *b, *out;
//     float *d_a;

//     a = (float*)malloc(sizeof(float) * N);

//     // Allocate device memory for a
//     Allocate Device Memory -> Return Buffer Pointer
//     cudaMalloc((void**)&d_a, sizeof(float) * N);

//     // Transfer data from host to device memory
//     cudaMemcpy(d_a, a, sizeof(float) * N, cudaMemcpyHostToDevice);

//     …
//     vector_add<<<1,1>>>(out, d_a, b, N);
//     …

//     // Cleanup after kernel execution
//     cudaFree(d_a);
//     free(a);
// }

enum class Modifier
{
    TEMPORARY,
    PERSISTANT,
};

typedef struct
{
    uint32_t bufferSize;
    VkBuffer buffer;
    VkDeviceMemory deviceMemory;
} BufferMemory;

typedef struct
{
    int32_t size;
    float *data;
} Floats;

typedef struct
{
    BufferMemory *bufferMemory;
    void *data;
    Modifier modifier;
} Argument;

typedef struct
{
    int32_t x, y, z;
} ParallelGroup;

// Instance //
inline VkInstance __instance;

// Devices //
inline std::vector<VkPhysicalDevice> __physicalDevices;
inline VkPhysicalDevice __physicalDevice;
inline VkDevice __device;
inline uint32_t __familyIndex = 0;

// Shader //
inline VkShaderModule __shaderModule;

// Memory //
// Type of memory
inline uint32_t __memoryTypeIndex = -1;
inline std::unordered_map<std::uintptr_t, BufferMemory *> __bufferMemory;

// Private Functionality //
// Allocate Memory
VkBuffer createBuffer(size_t bufferSize);
VkDeviceMemory createDeviceMemory(VkBuffer bufferSize);
BufferMemory *getAllocation(void *dataPtr);

// Create Pipeline Layout
VkDescriptorSetLayout createDescriptorSetLayout(uint32_t bindingCount);
VkDescriptorSetLayoutBinding createLayoutBinding(uint32_t binding);
VkDescriptorPoolSize createDescriptorPoolSize(uint32_t descriptorCount);
VkDescriptorPool createDescriptorPool(std::vector<VkDescriptorPoolSize> pools, uint32_t maxSets); // NumDescriptors * sizeof(Descriptor) + NumSets * sizeof(DescriptorSet);
std::vector<VkDescriptorSet> allocateDescriptorSets(VkDescriptorPool pool, std::vector<VkDescriptorSetLayout> layouts);
VkWriteDescriptorSet createDescriptorSet(VkDescriptorSet descriptorSet, size_t binding, VkBuffer buffer, size_t bufferSize);
void updateDescriptorSets(std::vector<VkWriteDescriptorSet> descriptorWrites, std::vector<VkCopyDescriptorSet> descriptorCopies);

// Create Pipelines
VkPipelineLayout createPipelineLayout(std::vector<VkDescriptorSetLayout> layouts);
VkPipelineCache createPipelineCache();
VkComputePipelineCreateInfo createComputePipelineCreateInfo(const char *funName, VkPipelineLayout pipelineLayout);
std::vector<VkPipeline> createPipelines(std::vector<VkComputePipelineCreateInfo> createInfos, VkPipelineCache pipelineCache);

// Create Command
VkCommandPool createCommandPool(uint32_t familyIndex);
std::vector<VkCommandBuffer> createCommandBuffers(VkCommandPool commandPool, uint32_t commandBufferCount);
void parseCommand(VkCommandBuffer commandBuffer, VkPipeline pipeline, VkPipelineLayout pipelineLayout, std::vector<VkDescriptorSet> descriptorSets, ParallelGroup group);

// Queue
VkFence createFence();
VkQueue getDeviceQueue(uint familyIndex, uint queueIndex);
void queueSubmit(VkQueue queue, std::vector<VkCommandBuffer> commandBuffers, VkFence fence);
void waitFences(std::vector<VkFence> fences, bool waitAll, uint timeout);
void resetFences(std::vector<VkFence> fences);
VkResult getFenceStatus(VkFence fence);

// Public Functionality //
// Setup
extern "C" void __setupInstance(bool isDebug);
extern "C" void __setupDevice();
extern "C" void __setupMemory();

extern "C" void *__allocate(void *dataPtr, uint32_t bufferSize);
extern "C" void __deAllocation(void *dataPtr);

// Add shader file
extern "C" void __shader(const char *filename);

// Run Command
extern "C" void __run(const char *functionName, ParallelGroup group, uint32_t paramCount, ...);

// Release Global Variable
extern "C" void __release();
