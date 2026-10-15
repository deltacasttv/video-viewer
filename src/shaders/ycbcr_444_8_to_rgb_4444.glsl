constexpr char const * fragment_shader_ycbcr_444_8_to_rgb_44444 = R"(#version 410

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
    // gl_FragCoord.xy is the centre (x + 0.5, y + 0.5) of the output pixel in the texture_width x texture_height
    // conversion viewport (bottom-left origin): truncating it gives the exact pixel with integer-only math on every
    // GPU (#42). y is flipped so that row 0 is the first line of the input buffer.
    ivec2 coords = ivec2(int(gl_FragCoord.x), texture_height - 1 - int(gl_FragCoord.y));
    vec3 ycbcr = texelFetch(input_texture, coords, 0).rgb;
    output_color = yuv2rgba(vec4(ycbcr.y, ycbcr.x, ycbcr.z, 1.0));
}
)";