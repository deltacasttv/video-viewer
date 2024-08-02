constexpr char const * compute_shader_bgr_444_8 = R"(

#version 430

layout(local_size_x = 8, local_size_y = 8) in;

layout(rgba8, binding = 0) uniform image2D img_output;
layout(rgba8, binding = 1) uniform image2D img_input;

void main() {
    int width = imageSize(img_output).x;
    ivec2 coords = ivec2(gl_GlobalInvocationID);
    float PxlIdx = coords.x + coords.y * width;
    vec4 bgr;
    vec4 rgba;
    rgba.w = 1.0;
    if (mod(PxlIdx, 4.0) < 1.0)
    {
       bgr = imageLoad(img_input, ivec2(coords.x*3.0/4.0 , coords.y));
       rgba.z = bgr.x;
       rgba.y = bgr.y;
       rgba.x = bgr.z;
    }
    else if (mod(PxlIdx, 4.0) < 2.0)
    {
       bgr = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0)  , coords.y));
       rgba.z = bgr.w;
       bgr = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0) + 1 , coords.y));
       rgba.y = bgr.x;
       rgba.x = bgr.y;
    }
    else if (mod(PxlIdx, 4.0) < 3.0)
    {
       bgr = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0) , coords.y));
       rgba.z = bgr.z;
       rgba.y = bgr.w;
       bgr = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0) + 1 , coords.y));
       rgba.x = bgr.x;
    }
    else if (mod(PxlIdx, 4.0) < 4.0)
    {
       bgr = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0) , coords.y));
       rgba.z = bgr.y;
       rgba.y = bgr.z;
       rgba.x = bgr.w;
    }

    imageStore(img_output, coords, rgba);
}
)";
