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

// Returns texel texel_x of input line `line` as a big-endian 32-bit word: each line is one row of the RGBA8 input
// texture and holds the bytes of the line in order, 4 per texel (r = first byte). The texel is clamped so that
// texelFetch is always in range.
uint read_be_word(int texel_x, int line)
{
    ivec2 texel = clamp(ivec2(texel_x, line), ivec2(0), textureSize(input_texture, 0) - 1);
    // v * 255.0 is within rounding error of an integer: adding 0.5 and truncating gives the exact byte.
    uvec4 bytes = uvec4(texelFetch(input_texture, texel, 0) * 255.0 + 0.5);
    return (bytes.r << 24) | (bytes.g << 16) | (bytes.b << 8) | bytes.a;
}

void main() {
    // gl_FragCoord.xy is the centre (x + 0.5, y + 0.5) of the output pixel in the texture_width x texture_height
    // conversion viewport (bottom-left origin): truncating it gives the exact pixel with integer-only math on every
    // GPU (#42). y is flipped so that row 0 is the first line of the input buffer.
    ivec2 coords = ivec2(int(gl_FragCoord.x), texture_height - 1 - int(gl_FragCoord.y));

    // A pixel pair is 4 samples of 10 bits packed MSB first (Cb, Y of the even pixel, Cr, Y of the odd pixel):
    // bytes 5 * pair to 5 * pair + 4 of the line, which lie in two consecutive texels.
    int first_byte = (coords.x / 2) * 5;
    uint word0 = read_be_word(first_byte / 4, coords.y);
    uint word1 = read_be_word(first_byte / 4 + 1, coords.y);
    // Align the pair on bit 31: its first 32 bits in high, its last 8 bits in the top byte of low.
    // (A shift by 32 is undefined in GLSL, hence the shift == 0 case.)
    int shift = (first_byte % 4) * 8;
    uint high = (shift == 0) ? word0 : ((word0 << shift) | (word1 >> (32 - shift)));
    uint low = word1 << shift;

    uint y_code = ((coords.x & 1) == 0) ? ((high >> 12) & 0x3FFu) : (((high & 0x3u) << 8) | (low >> 24));

    vec4 yuvk;
    yuvk.x = float(y_code) / 1023.0;
    yuvk.y = float(high >> 22) / 1023.0;
    yuvk.z = float((high >> 2) & 0x3FFu) / 1023.0;
    yuvk.w = 1.0;

    output_color = yuv2rgba(yuvk);
}
)";