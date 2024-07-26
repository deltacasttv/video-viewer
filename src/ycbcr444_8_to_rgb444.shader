constexpr char const * compute_shader_yuv_444_8 = R"(
{
#version 430

layout(local_size_x = 8, local_size_y = 8) in;

layout(rgba8, binding = 0) uniform image2D img_output;
layout(rgba8, binding = 1) uniform image2D img_input;

vec4 yuv2rgba(vec4 yuvk)
{
    vec4 rgba;
    bool bt_709 = true; //TODO: pass this information from the code

    rgba.x = 1.164 * (yuvk.x - 16.0 / 255.0) + ((float(bt_709) * 1.793) + (float(!bt_709)) * (1.596)) * (yuvk.z - 128.0 / 255.0);
    rgba.y = 1.164 * (yuvk.x - 16.0 / 255.0) - ((float(bt_709) * 0.534) + (float(!bt_709)) * (0.813)) * (yuvk.z - 128.0 / 255.0) - 0.213 * (yuvk.y - 128.0 / 255.0);
    rgba.z = 1.164 * (yuvk.x - 16.0 / 255.0) + ((float(bt_709) * 2.115) + (float(!bt_709)) * (2.018)) * (yuvk.y - 128.0 / 255.0);
    rgba.w = 1.0;
    return rgba;
}

void main() {
    int width = imageSize(img_output).x;
    ivec2 coords = ivec2(gl_GlobalInvocationID);
    float PxlIdx = coords.x + coords.y * width;
    vec4 uyvk;
    vec4 yuvk;
    yuvk.w = 1.0;
    if (mod(PxlIdx, 4.0) < 1.0)
    {
       uyvk = imageLoad(img_input, ivec2(coords.x*3.0/4.0 , coords.y));
       yuvk.y = uyvk.x;
       yuvk.x = uyvk.y;
       yuvk.z = uyvk.z;
    }
    else if (mod(PxlIdx, 4.0) < 2.0)
    {
       uyvk = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0)  , coords.y));
       yuvk.y = uyvk.w;
       uyvk = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0) + 1 , coords.y));
       yuvk.x = uyvk.x;
       yuvk.z = uyvk.y;
    }
    else if (mod(PxlIdx, 4.0) < 3.0)
    {
       uyvk = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0) , coords.y));
       yuvk.y = uyvk.z;
       yuvk.x = uyvk.w;
       uyvk = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0) + 1 , coords.y));
       yuvk.z = uyvk.x;
    }
    else if (mod(PxlIdx, 4.0) < 4.0)
    {
       uyvk = imageLoad(img_input, ivec2(int(coords.x*3.0/4.0) , coords.y));
       yuvk.y = uyvk.y;
       yuvk.x = uyvk.z;
       yuvk.z = uyvk.w;
    }

    vec4 pixel = yuv2rgba(yuvk);

    imageStore(img_output, coords, pixel);
}
)";
