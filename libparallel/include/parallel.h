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

// Functionality //
// Setup
extern "C" void __setupInstance(bool isDebug);
extern "C" void __setupDevice();
extern "C" void __setupMemory();

// Create Pipeline Layout
VkDescriptorSetLayout createDescriptorSetLayout(uint32_t bindingCount);
VkDescriptorSetLayoutBinding createLayoutBinding(uint32_t binding);
VkPipelineLayout createPipelineLayout(std::vector<VkDescriptorSetLayout> layouts);

// Create Pipeline
VkPipelineCache createPipelineCache();
VkComputePipelineCreateInfo createComputePipelineCreateInfo(const char *funName, VkPipelineLayout pipelineLayout);
std::vector<VkPipeline> createPipelines(std::vector<VkComputePipelineCreateInfo> createInfos, VkPipelineCache pipelineCache);

// Allocate Memory
VkBuffer createBuffer(size_t bufferSize);
VkDeviceMemory createDeviceMemory(VkBuffer bufferSize);

BufferMemory *getAllocation(void *dataPtr);

extern "C" void *__allocate(void *dataPtr, uint32_t bufferSize);
extern "C" void __deAllocation(void *dataPtr);

// Add shader file
extern "C" void __shader(const char *filename);

// Run Command
extern "C" void __run(const char *functionName, ParallelGroup group, uint32_t paramCount, ...);

// Release Global Variable
extern "C" void __release();
