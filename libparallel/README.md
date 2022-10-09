# Vulkan Parallel Compute

This library currently being developed for internal use on Weasel Language (https://github/aglis_lab/Weasel).

But other independant project can use this library for developing their parallel system or application.


## Usage

    __setupInstance(true);
    __setupDevice();
    __setupMemory();
    __shader(<spirv file path>);

    auto param1 = __allocate(<data pointer or use nullptr>, <buffer Size or memory size in bytes>);

    __run(<kernel name>, ParallelGroup{<x>, <y>, <z>}, <kernel parameter count>, <...parameters>);

    __deAllocation(param1);
    __release();

## Example

    #include <iostream>

    #include <parallel.h>

    int main()
    {
        __setupInstance(true);
        __setupDevice();
        __setupMemory();
        __shader("/Users/zaen/Projects/Open Source/weasel/test/vulkan-test/comp.spv");

        auto numElement = 10;
        auto paramCount = 2;
        Floats floats1;
        floats1.size = numElement;
        floats1.data = (float *)malloc(numElement * sizeof(float));

        Floats floats2;
        floats2.size = numElement;
        floats2.data = (float *)__allocate(nullptr, numElement * sizeof(float));

        for (size_t i = 0; i < numElement; i++)
        {
            floats1.data[i] = (float)i + 1;
        }

        __run("main", ParallelGroup{numElement, 1, 1}, paramCount, &floats1, &floats2);

        std::cout << "Values\n";
        for (size_t i = 0; i < numElement; i++)
        {
            std::cout << floats1.data[i] << " ";
        }
        std::cout << std::endl;
        for (size_t i = 0; i < numElement; i++)
        {
            std::cout << floats2.data[i] << " ";
        }
        std::cout << std::endl;

        free(floats1.data);
        __deAllocation((void *)floats2.data);

        __release();
    }
