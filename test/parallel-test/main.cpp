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
    floats1.data = (float *)__allocate(nullptr, numElement * sizeof(float));

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

    // __run("main", ParallelGroup{numElement, 1, 1}, paramCount, &floats2, &floats1);

    // std::cout << "Values\n";
    // for (size_t i = 0; i < numElement; i++)
    // {
    //     std::cout << floats1.data[i] << " ";
    // }
    // std::cout << std::endl;
    // for (size_t i = 0; i < numElement; i++)
    // {
    //     std::cout << floats2.data[i] << " ";
    // }
    // std::cout << std::endl;

    // free(floats1.data);
    // __deAllocation((void *)floats2.data);

    // __release();
}
