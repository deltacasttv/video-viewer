constexpr char const * compute_shader_422_8 = R"(
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
    ivec2 coords = ivec2(gl_GlobalInvocationID);
    vec4 uyvy = imageLoad(img_input, ivec2(coords.x / 2, coords.y));
    bool odd_column = (mod(coords.x, 2) >= 1.0);
    vec4 yuvk;
    yuvk.x = (float(!odd_column)) * uyvy.y + (float(odd_column)) * uyvy.w;
    yuvk.y = uyvy.x;
    yuvk.z = uyvy.z;
    yuvk.w = 1.0;

    vec4 pixel = yuv2rgba(yuvk);

    imageStore(img_output, coords, pixel);
}
)";