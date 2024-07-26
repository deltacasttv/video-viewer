constexpr char const * compute_shader_bgr_444_8_le_msb = R"(
#version 430

layout(local_size_x = 8, local_size_y = 8) in;

layout(rgba8, binding = 0) uniform image2D img_output;
layout(rgba8, binding = 1) uniform image2D img_input;

void main()
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID);
    vec4 bgra = imageLoad(img_input, ivec2(pixelCoords.x,pixelCoords.y));
    vec4 rgba = vec4(bgra.z, bgra.y, bgra.x, 1.0);
    imageStore(img_output, pixelCoords, rgba);
};
)";
