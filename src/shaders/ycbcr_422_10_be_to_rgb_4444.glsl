constexpr char const * fragment_shader_ycbcr_422_10_be_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform int texture_width;
uniform int texture_height;

uniform sampler2D input_texture;

uniform bool bt_709;

vec4 yuv2rgba(vec4 yuvk)
{
    vec4 rgba;

    float y = yuvk.x- (16.0 / 255.0);
    float cb = yuvk.y - (128.0 / 255.0);
    float cr = yuvk.z - (128.0 / 255.0);

    if(bt_709)
    {
        rgba.x = 1.164 * y + 1.793 * cr;
        rgba.y = 1.164 * y - 0.534 * cb - 0.213 * cr;
        rgba.z = 1.164 * y + 2.115 * cb;
    }
    else
    {
        rgba.x = 1.164 * y + 1.596 * cr;
        rgba.y = 1.164 * y - 0.813 * cr - 0.391 * cb;
        rgba.z = 1.164 * y + 2.018 * cb;
    }
    rgba.a = 1.0;
    return rgba;
}

void main() {

    ivec2 texture_size = textureSize(input_texture, 0);
    ivec2 coords = ivec2(round(texture_coordinates.x * texture_width), round(texture_coordinates.y * texture_height));

    vec4 uyvy;
    vec4 uyvy2;

    float pixel_index = coords.x + coords.y * texture_size.x;
    vec4 yuvk;
    yuvk.w = 1.0;

    if(mod(pixel_index, 8.0) < 1.0)
    {
        uyvy = texelFetch(input_texture, ivec2(int(coords.x * 5.0 / 8.0), coords.y), 0);
        yuvk.x = ((mod(uyvy.y * 255.0,64) * 16) + (int(uyvy.z * 255.0 / 16.0))) / 1023.0;
        yuvk.y = ((uyvy.x * 255.0 * 4.0) + (int(uyvy.y * 255.0 / 64.0))) / 1023.0;
        yuvk.z = ((mod(uyvy.z * 255.0,16.0) * 64.0) + (int(uyvy.w * 255.0 / 4.0))) / 1023.0;
    }
    else if (mod(pixel_index, 8.0) < 2.0)
    {
        uyvy = texelFetch(input_texture, ivec2(int(coords.x * 5.0 / 8.0), coords.y), 0);
        yuvk.y = ((uyvy.x * 255.0 * 4.0) + (uyvy.y * 255.0 / 64.0)) / 1023.0;
        yuvk.z = ((mod(uyvy.z * 255.0,16.0) * 64.0) + (uyvy.w * 255.0 / 4.0)) / 1023.0;
        uyvy2 = texelFetch(input_texture, ivec2(int(coords.x * 5.0 / 8.0) + 1, coords.y), 0);
        yuvk.x = (uyvy2.x * 255.0 + (mod(uyvy.w * 255.0,4.0) * 256.0)) / 1023.0;
    }
    else if (mod(pixel_index, 8.0) < 3.0)
    {
        uyvy = texelFetch(input_texture, ivec2(int(coords.x * 5.0 / 8.0), coords.y), 0);
        yuvk.x = ((mod(uyvy.z * 255.0, 64.0) * 16.0) + (uyvy.w * 255.0 / 16.0)) / 1023.0;
        yuvk.y = ((uyvy.y * 255.0 * 4.0) + (uyvy.z * 255.0 / 64.0)) / 1023.0;
        uyvy2 = texelFetch(input_texture, ivec2(int(coords.x * 5.0 / 8.0) + 1, coords.y), 0);
        yuvk.z = ((mod(uyvy.w * 255.0,16.0) * 64.0) + (uyvy2.x * 255.0 / 4.0)) / 1023.0;
    }
    else if (mod(pixel_index, 8.0) < 4.0)
    {
        uyvy = texelFetch(input_texture, ivec2(int(coords.x * 5.0 / 8.0) + 1, coords.y), 0);
        yuvk.x = (uyvy.y * 255.0 + (mod(uyvy.x * 255.0,4.0) * 256.0)) / 1023.0;
        uyvy2 = texelFetch(input_texture, ivec2(int(coords.x * 5.0 / 8.0), coords.y), 0);
        yuvk.y = ((uyvy2.y * 255.0 * 4.0) + (uyvy2.z * 255.0 / 64.0)) / 1023.0;
        yuvk.z = ((mod(uyvy2.w * 255.0,16.0) * 64.0) + (uyvy.x * 255.0 / 4.0)) / 1023.0;
    }
    else if (mod(pixel_index, 8.0) < 5.0)
    {
        uyvy = texelFetch(input_texture,  ivec2(int(coords.x * 5.0 / 8.0), coords.y), 0);
        uyvy2 = texelFetch(input_texture, ivec2(int(coords.x * 5.0 / 8.0) + 1 , coords.y), 0);
        yuvk.x = ((mod(uyvy.w * 255.0, 64.0) * 16.0) + (uyvy2.x * 255.0 / 16.0)) / 1023.0;
        yuvk.y = ((uyvy.z * 255.0 * 4.0) + (uyvy.w * 255.0 / 64.0)) / 1023.0;
        yuvk.z = ((mod(uyvy2.x * 255.0,16.0) * 64.0) + (uyvy2.y * 255.0 / 4.0)) / 1023.0;
    }
    else if (mod(pixel_index, 8.0) < 6.0)
    {
        uyvy = texelFetch(input_texture,  ivec2(int(coords.x * 5.0 / 8.0), coords.y), 0);
        yuvk.x = (uyvy.z * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;
        uyvy2 = texelFetch(input_texture,  ivec2(int(coords.x * 5.0 / 8.0) - 1 , coords.y), 0);
        yuvk.y = ((uyvy2.z * 255.0 * 4.0) + (uyvy2.w * 255.0 / 64.0)) / 1023.0;
        yuvk.z = ((mod(uyvy.x * 255.0,16.0) * 64.0) + (uyvy.y * 255.0 / 4.0)) / 1023.0;
    }
    else if (mod(pixel_index, 8.0) < 7.0)
    {
        uyvy = texelFetch(input_texture,  ivec2(int(coords.x * 5.0 / 8.0), coords.y), 0);
        uyvy2 = texelFetch(input_texture, ivec2(int(coords.x * 5.0 / 8.0) + 1 , coords.y), 0);
        yuvk.x = ((mod(uyvy2.x * 255.0, 64.0) * 16.0) + (uyvy2.y * 255.0 / 16.0)) / 1023.0;
        yuvk.y = ((uyvy.w * 255.0 * 4.0) + (uyvy2.x * 255.0 / 64.0)) / 1023.0;
        yuvk.z = ((mod(uyvy2.y * 255.0,16.0) * 64.0) + (uyvy2.z * 255.0 / 4.0)) / 1023.0;
    }
    else
    {
        uyvy = texelFetch(input_texture,  ivec2(int(coords.x * 5.0 / 8.0), coords.y), 0);
        uyvy2 = texelFetch(input_texture, ivec2(int(coords.x * 5.0 / 8.0) - 1, coords.y), 0);
        yuvk.x = (uyvy.w * 255.0 + (mod(uyvy.z * 255.0,4.0) * 256.0)) / 1023.0;
        yuvk.y = ((uyvy2.w * 255.0 * 4.0) + (uyvy.x * 255.0 / 64.0)) / 1023.0;
        yuvk.z = ((mod(uyvy.y * 255.0,16.0) * 64.0) + (uyvy.z * 255.0 / 4.0)) / 1023.0;
    }

    output_color = yuv2rgba(yuvk);
}
)";