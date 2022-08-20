#include <spirv_glsl.hpp>
#include <iostream>
#include <utility>

int main()
{
    // Read SPIR-V from disk or similar.
    std::vector<uint32_t> spirv_binary = {};

    spirv_cross::CompilerGLSL glsl(std::move(spirv_binary));

    // The SPIR-V is now parsed, and we can perform reflection on it.
    spirv_cross::ShaderResources resources = glsl.get_shader_resources();

    // Get all sampled images in the shader.
    for (auto &resource : resources.sampled_images)
    {
        unsigned set = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
        unsigned binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
        printf("Image %s at set = %u, binding = %u\n", resource.name.c_str(), set, binding);

        // Modify the decoration to prepare it for GLSL.
        glsl.unset_decoration(resource.id, spv::DecorationDescriptorSet);

        // Some arbitrary remapping if we want.
        glsl.set_decoration(resource.id, spv::DecorationBinding, set * 16 + binding);
    }

    // Set some options.
    spirv_cross::CompilerGLSL::Options options;
    options.version = 310;
    options.es = true;
    glsl.set_common_options(options);

    // Compile to GLSL, ready to give to GL driver.
    std::string source = glsl.compile();

    std::cout << source << std::endl;
}
