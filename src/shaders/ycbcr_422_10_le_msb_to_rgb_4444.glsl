constexpr char const * fragment_shader_ycbcr_422_10_le_msb_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform sampler2D input_texture;

uniform int output_width;
uniform int output_height;

uniform bool bt_709;

vec4 yuv2rgba(vec4 yuvk)
{
    vec4 rgba;

    rgba.x = 1.164 * (yuvk.x - 16.0 / 255.0) + ((float(bt_709) * 1.793) + (float(!bt_709)) * (1.596)) * (yuvk.z - 128.0 / 255.0);
    rgba.y = 1.164 * (yuvk.x - 16.0 / 255.0) - ((float(bt_709) * 0.534) + (float(!bt_709)) * (0.813)) * (yuvk.z - 128.0 / 255.0) - 0.213 * (yuvk.y - 128.0 / 255.0);
    rgba.z = 1.164 * (yuvk.x - 16.0 / 255.0) + ((float(bt_709) * 2.115) + (float(!bt_709)) * (2.018)) * (yuvk.y - 128.0 / 255.0);
    rgba.w = 1.0;
    return rgba;
}

void main() {
    int width = textureSize(input_texture, 0).x / 2;
    ivec2 coords = ivec2(texture_coordinates);
    vec4 uyvy;
    float PxlIdx = coords.x + coords.y * width;
    vec4 yuvk;
    yuvk.w = 1.0;
    float x_threshold = mod(float(coords.y),3.0) * width;

    if (mod(PxlIdx, 6.0) < 1.0)
    {
        uyvy = texelFetch(input_texture, ivec2(int((coords.x + x_threshold)*2.0/3.0), coords.y / 3), 0);
        yuvk.x = ((uyvy.y * 255.0 / 4.0) + (mod(uyvy.z * 255.0,16.0) * 64.0)) / 1023.0;
        yuvk.y = (uyvy.x * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;
        yuvk.z = ((uyvy.z * 255.0 / 16.0) + (mod(uyvy.w * 255.0,64.0) * 16.0)) / 1023.0;
    }
    else if (mod(PxlIdx, 6.0) < 2.0)
    {
        uyvy = texelFetch(input_texture, ivec2(int((coords.x + x_threshold)*2.0/3.0), coords.y / 3.0), 0);
        yuvk.y = (uyvy.x * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;
        yuvk.z = ((uyvy.z * 255.0 / 16.0) + (mod(uyvy.w * 255.0,64.0) * 16.0)) / 1023.0;
        uyvy = texelFetch(input_texture, ivec2(int((coords.x + x_threshold)*2.0/3.0) + 1, coords.y / 3.0), 0);
        yuvk.x = (uyvy.x * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;
    }
    else if (mod(PxlIdx, 6.0) < 3.0)
    {
        uyvy = texelFetch(input_texture, ivec2(int((coords.x + x_threshold)*2.0/3.0), coords.y / 3.0), 0);
        yuvk.x = ((uyvy.z * 255.0 / 16.0) + (mod(uyvy.w * 255.0,64.0) * 16.0)) / 1023.0;
        yuvk.y = ((uyvy.y * 255.0 / 4.0) + (mod(uyvy.z * 255.0,16.0) * 64.0)) / 1023.0;
        uyvy = texelFetch(input_texture, ivec2(int((coords.x + x_threshold)*2.0/3.0)+1, coords.y / 3.0), 0);
        yuvk.z = (uyvy.x * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;
    }
    else if (mod(PxlIdx, 6.0) < 4.0)
    {
        uyvy = texelFetch(input_texture, ivec2(int((coords.x + x_threshold)*2.0/3.0), coords.y / 3.0), 0);
        yuvk.x = ((uyvy.y * 255.0 / 4.0) + (mod(uyvy.z * 255.0,16.0) * 64.0)) / 1023.0;
        yuvk.z = (uyvy.x * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;
        uyvy = texelFetch(input_texture, ivec2(int((coords.x + x_threshold)*2.0/3.0)-1, coords.y / 3.0), 0);
        yuvk.y = ((uyvy.y * 255.0 / 4.0) + (mod(uyvy.z * 255.0,16.0) * 64.0)) / 1023.0;
    }
    else if (mod(PxlIdx, 6.0) < 5.0)
    {
        uyvy = texelFetch(input_texture, ivec2(int((coords.x + x_threshold)*2.0/3.0), coords.y / 3.0), 0);
        yuvk.y = ((uyvy.z * 255.0 / 16.0) + (mod(uyvy.w * 255.0,64.0) * 16.0)) / 1023.0;
        uyvy = texelFetch(input_texture, ivec2(int((coords.x + x_threshold)*2.0/3.0)+1 , coords.y / 3.0), 0);
        yuvk.x = (uyvy.x * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;
        yuvk.z = ((uyvy.y * 255.0 / 4.0) + (mod(uyvy.z * 255.0,16.0) * 64.0)) / 1023.0;
    }
    else
    {
        uyvy = texelFetch(input_texture, ivec2(int((coords.x + x_threshold)*2.0/3.0), coords.y / 3.0), 0);
        yuvk.x = ((uyvy.z * 255.0 / 16.0) + (mod(uyvy.w * 255.0,64.0) * 16.0)) / 1023.0;
        yuvk.z = ((uyvy.y * 255.0 / 4.0) + (mod(uyvy.z * 255.0,16.0) * 64.0)) / 1023.0;
        uyvy = texelFetch(input_texture, ivec2(int((coords.x + x_threshold)*2.0/3.0)-1, coords.y / 3.0), 0);
        yuvk.y = ((uyvy.z * 255.0 / 16.0) + (mod(uyvy.w * 255.0,64.0) * 16.0)) / 1023.0;
    }

    output_color = yuv2rgba(yuvk);
}
)";