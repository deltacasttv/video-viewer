constexpr char const * fragment_shader_ycbcr_422_8_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform sampler2D input_texture;

vec4 yuv2rgba(vec3 ycbcr, bool bt_709)
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
    bool bt_709 = true; //TODO: pass this information from the code
    ivec2 texture_size = textureSize(input_texture, 0);
    ivec2 texture_coords = ivec2(texture_size.x * texture_coordinates.x, texture_size.y * texture_coordinates.y);
    bool is_odd = (texture_coords.x % 2) == 1;
    vec4 ycbcr = texture(input_texture, texture_coordinates).rgba; // u, y, v, y
    if(is_odd)
    {
        output_color = yuv2rgba(ycbcr.xyz, bt_709);
    }
    else
    {
        output_color = yuv2rgba(ycbcr.xwz, bt_709);
    }
}
)";