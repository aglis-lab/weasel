#include <vulkan/vulkan.hpp>

#include <vector>

// Instance //
inline VkInstance __instance;

// Devices //
inline std::vector<VkPhysicalDevice> __physicalDevices;
inline VkPhysicalDevice __physicalDevice;
inline VkDevice __device;
inline uint32_t __familyIndex = -1;

// Memory //
// Type of memory
inline uint32_t __memoryTypeIndex = -1;

// Functionality //
extern "C" void __setupInstance();
extern "C" void __setupDevice();
extern "C" void __setupMemory();
extern "C" void __release();

// Memory Device //
// extern "C" void __allocateDeviceMemory();
