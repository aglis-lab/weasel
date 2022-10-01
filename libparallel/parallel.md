## Parallel Usage

    impl parallel
    fun mul(x *int, y *int) {
        // BODY
    }
    mul<<x, y, z>>(deviceX, deviceY)

## Actions
### 1. Create Instance
### 2. Get Physical Device Properties
### 3. Create Device
### 4. Create Buffer

- Parallel Function Params

### 5. Get Buffer Memory Requirements

- Buffer

### 6. Get Physical Device Memory Properties

- Physical Device

### 7. Create Shader Module

- Parallel Function Body

### 8. Create Descriptor Set Layout

- Parallel Functiion Params -> Layout Binding GLSL

### 9. Create Pipeline Layout

- Parallel Function Params -> Layout Set GLSL

### 10. Create Pipeline Cache

### 11. Create Compute Pipelines

- Parallel Function Name
- Shader Module -> 7
- Pipeline Cache -> 10

### 12. Create Descriptor Pool

- Parallel Function Params
- Descriptor Count -> 8

### 13. Allocate Descriptor Sets

- Layout -> 9
- Descriptor Pool -> 12

### 14. Update Descriptor Sets

- Parallel Function Params
- DstBinding -> Parallel Function Params (Layout Binding GLSL)
- Descriptor Count -> 13
- DstSet -> 13
- Buffer -> 4

### 15. Create Command Poll

- FamilyIndex -> 2
- CommandBufferCount -> Parallel Function (GLSL)

### 16. Allocate Command Buffers

- CommandPool -> 15
- => CommandBuffers

### 17. Command Buffer

### 18. Begin Command Buffer

- CommandBuffer -> 16

### 19. Cmd Bind Pipeline

- Command Buffer -> 16
- Compute Pipeline -> 11

### 20. Cmd Bind Descriptor Sets

<!-- VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet *pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t *pDynamicOffsets -->
- Command Buffer -> 16
- Pipeline -> 9
- FirstSet -> Function Parallel Params (set)
- DescriptorSetCount -> 13
- DescriptorSets -> 13

<!-- uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ -->
### 21. Cmd Dispatch

- Command Buffer -> 16
- GroupCountX -> Function Parallel (X Group)
- GroupCountY -> Function Parallel (Y Group)
- GroupCountZ -> Function Parallel (Z Group)

### 22. End Command Buffer

- Command Buffer -> 16


### 23. Create Fence

- Device -> 3

### 24. Get Device Queue

- Device -> 3
- FamilyIndex -> 2
<!-- - QueueIndex ->  -->

### 25. Allocate Memories

- Device -> 3
- AllocationSize -> Get Buffer Memory Requirements

### 26. Bind Buffer Memory

- Device -> 3
- Buffer -> 4
- DeviceMemory -> 25

<!-- VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void **ppData -->
### 27. Map Memory

- Device -> 3
- Device Memory -> 25
- DeviceSize (offset)
- DeviceSize (size) -> Get Buffer Memory Requirements
- MemoryMapFlags
- Data (Buffer) -> 4

### 28. Get Device Queue

- Device -> 3
- FamilyIndex -> 2
- QueueIndex
- => Queue

### 29. Submit Queue (When you submit the queue, it's already run it)

- Queue -> 28
- Fence -> 23

### 3. Wait For Fences (Waiting the queue finish)

- Device -> 3
- Fence -> 23
