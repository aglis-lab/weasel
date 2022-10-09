#include <fstream>
#include <vector>
#include <iostream>

std::vector<uint32_t> readBinaryFile(const char *filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cout << "File not openned\n";
        exit(1);
    }

    std::vector<char> temp((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

    file.close();

    // Copy data from the char-vector to a new uint32_t-vector
    std::vector<uint32_t> spv(temp.size());

    memcpy(spv.data(), temp.data(), temp.size());

    return spv;
}
