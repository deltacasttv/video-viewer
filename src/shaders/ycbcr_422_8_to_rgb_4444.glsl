constexpr char const * fragment_shader_ycbcr_422_8_to_rgb_44444 = R"(#version 410

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
    ivec2 texture_coords = ivec2(round(texture_coordinates.x * texture_width), round(texture_coordinates.y * texture_height));
    bool is_odd = (texture_coords.x % 2) == 1;

    vec4 ycbcr = texelFetch(input_texture, ivec2(round(texture_coords.x / 2.0), round(texture_coords.y)), 0);
    if(is_odd)
    {
        output_color = yuv2rgba(vec4(ycbcr.yxz, 1.0));
    }
    else
    {
        output_color = yuv2rgba(vec4(ycbcr.wxz, 1.0));
    }
}
)";