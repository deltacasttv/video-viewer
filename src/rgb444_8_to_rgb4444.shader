constexpr char const * compute_shader_rgb_444_8 =
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
"    vec4 rgb;\n"
"    vec4 rgba;\n"
"    rgba.w = 1.0;\n"
"    if (mod(PxlIdx, 4.0) < 1.0)\n"
"    {\n"
"       rgb = imageLoad(img_input, ivec2(coords.x*3.0/4.0 , coords.y));\n"
"       rgba.x = rgb.x;\n"
"       rgba.y = rgb.y;\n"
"       rgba.z = rgb.z;\n"
"    }\n"
"    else if (mod(PxlIdx, 4.0) < 2.0)\n"
"    {\n"
"       rgb = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0)  , coords.y));\n"
"       rgba.x = rgb.w;\n"
"       rgb = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0) + 1 , coords.y));\n"
"       rgba.y = rgb.x;\n"
"       rgba.z = rgb.y;\n"
"    }\n"
"    else if (mod(PxlIdx, 4.0) < 3.0)\n"
"    {\n"
"       rgb = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0) , coords.y));\n"
"       rgba.x = rgb.z;\n"
"       rgba.y = rgb.w;\n"
"       rgb = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0) + 1 , coords.y));\n"
"       rgba.z = rgb.x;\n"
"    }\n"
"    else if (mod(PxlIdx, 4.0) < 4.0)\n"
"    {\n"
"       rgb = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0) , coords.y));\n"
"       rgba.x = rgb.y;\n"
"       rgba.y = rgb.z;\n"
"       rgba.z = rgb.w;\n"
"    }\n"
"\n"
"    imageStore(img_output, coords, rgba);\n"
"}\n"
}; 