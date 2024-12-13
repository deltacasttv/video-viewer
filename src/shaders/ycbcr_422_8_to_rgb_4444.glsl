constexpr char const * fragment_shader_ycbcr_422_8_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform sampler2D input_texture;

uniform bool bt_709;

vec4 yuv2rgba(vec4 yuvk)
{
    vec4 rgba;

    float y = (ycbcr.y * 255.0) - 16.0; // from 0-1 to 0-255
    float cb = (ycbcr.x * 255.0) - 128.0;
    float cr = (ycbcr.z * 255.0) - 128.0;

    if(bt_709)
    {
        rgba.x = (1.164 * y + 1.793 * cr) / 255.0;
        rgba.y = (1.164 * y - 0.534 * cb - 0.213 * cr) / 255.0;
        rgba.z = (1.164 * y + 2.115 * cb) / 255.0;
    }
    else
    {
        rgba.x = (1.164 * y + 1.596 * cr) / 255.0;
        rgba.y = (1.164 * y - 0.813 * cr - 0.391 * cb) / 255.0;
        rgba.z = (1.164 * y + 2.018 * cb) / 255.0;
    }
    rgba.a = 1.0;
    return rgba;
}

void main() {
    ivec2 texture_size = textureSize(input_texture, 0);
    ivec2 texture_coords = ivec2(texture_size.x * texture_coordinates.x, texture_size.y * texture_coordinates.y);
    bool is_odd = (texture_coords.x % 2) == 1;

    vec4 ycbcr = texelFetch(input_texture, ivec2(round(texture_coordinates.x / 2.0), round(texture_coordinates.y)), 0);
    if(is_odd)
    {
        output_color = yuv2rgba(vec4(ycbcr.xyz, 1.0));
    }
    else
    {
        output_color = yuv2rgba(vec4(ycbcr.xwz, 1.0));
    }
}
)";