#include <spirv_glsl.hpp>
#include <spirv_msl.hpp>
#include <spirv_hlsl.hpp>
#include <spirv_parser.hpp>
#include <iostream>
#include <fstream>
#include <utility>

// Read a binary file into a buffer (e.g. a SPIRV file)
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
    std::vector<uint32_t> spv(spirv.size() / 4);
    memcpy(spv.data(), spirv.data(), spirv.size());

    return spv;
}

int main()
{
    // Read SPIR-V from disk or similar.
    std::vector<uint32_t> spirv_binary = readBinaryFile("/Users/zaen/Projects/Open Source/weasel/test/cross-test/square.spv");
    spirv_cross::CompilerGLSL glsl(spirv_binary);
    spirv_cross::CompilerMSL mls(spirv_binary);
    spirv_cross::CompilerHLSL hlsl(spirv_binary);

    // Set MLS Options
    spirv_cross::CompilerMSL::Options mlsOptions;
    mls.set_msl_options(mlsOptions);

    // Set GLSL options.
    spirv_cross::CompilerGLSL::Options options;
    options.version = 310;
    options.es = true;
    glsl.set_common_options(options);

    // Set HLSL options.
    // spirv_cross::CompilerHLSL::Options hlslOptions;
    // hlsl.set_common_options(hlslOptions);

    // Compile to GLSL, ready to give to GL driver.
    std::string source = glsl.compile();
    std::cout << "========== GLSL ==========\n";
    std::cout << source << std::endl;

    // Compile to MLS
    std::string mlsSource = mls.compile();
    std::cout << "========== MLS ==========\n";
    std::cout << mlsSource << std::endl;

    // Compile to HLSL
    std::string hlslSource = hlsl.compile();
    std::cout << "========== HLSL ==========\n";
    std::cout << hlslSource << std::endl;
}
