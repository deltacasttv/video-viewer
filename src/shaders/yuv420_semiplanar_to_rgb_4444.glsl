constexpr char const * fragment_shader_yuv420_semiplanar_to_rgb_4444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform int texture_width;
uniform int texture_height;

uniform sampler2D input_texture;
uniform sampler2D uv_texture;

uniform bool bt_709;

vec4 yuv2rgba(vec4 yuvk)
{
    vec4 rgba;

    float y = yuvk.x - (16.0 / 255.0);
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
    ivec2 coords = ivec2(round(texture_coordinates.x * (texture_width - 1)), round(texture_coordinates.y * (texture_height - 1)));

    float y = texelFetch(input_texture, coords, 0).r;

    ivec2 uv_coords = ivec2(coords.x / 2, coords.y / 2);
    vec2 uv = texelFetch(uv_texture, uv_coords, 0).rg;

    vec4 yuvk = vec4(y, uv.r, uv.g, 1.0);
    output_color = yuv2rgba(yuvk);
}
)";