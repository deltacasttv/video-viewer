constexpr char const * compute_shader_bgr_444_8_le_msb =
{
"#version 430\n"
"\n"
"layout(local_size_x = 8, local_size_y = 8) in;\n"
"\n"
"layout(rgba8, binding = 0) uniform image2D img_output;\n"
"layout(rgba8, binding = 1) uniform image2D img_input;\n"
"\n"
"void main() {\n"
"    int width = imageSize(img_output).x;\n"
"    ivec2 coords = ivec2(gl_GlobalInvocationID);\n"
"    float PxlIdx = coords.x + coords.y * width;\n"
"    vec4 bgr;\n"
"    vec4 rgba;\n"
"    rgba.w = 1.0; // Alpha channel is always fully opaque\n"
"\n"
"    // Calculate the index for the 32-bit padded input\n"
"    int inputIdx = int(PxlIdx) / 3;\n"
"\n"
"    // Load the 32-bit chunk containing the BGR 444 data\n"
"    vec4 packedData = imageLoad(img_input, ivec2(inputIdx, coords.y));\n"
"\n"
"    // Extract BGR components based on the pixel index within the 32-bit chunk\n"
"    int localIdx = int(PxlIdx) % 3;\n"
"    if (localIdx == 0) {\n"
"        rgba.x = packedData.z & 0xF0; // Red\n"
"        rgba.y = (packedData.z & 0x0F) << 4; // Green\n"
"        rgba.z = packedData.y & 0xF0; // Blue\n"
"    } else if (localIdx == 1) {\n"
"        rgba.x = packedData.y & 0x0F; // Red\n"
"        rgba.y = packedData.w & 0xF0; // Green\n"
"        rgba.z = (packedData.w & 0x0F) << 4; // Blue\n"
"    } else if (localIdx == 2) {\n"
"        rgba.x = packedData.x & 0xF0; // Red\n"
"        rgba.y = (packedData.x & 0x0F) << 4; // Green\n"
"        rgba.z = packedData.z & 0xF0; // Blue\n"
"    }\n"
"\n"
"    // Normalize the components to the range [0, 1]\n"
"    rgba.xyz /= 255.0;\n"
"\n"
"    // Store the result\n"
"    imageStore(img_output, coords, rgba);\n"
"}\n"
};