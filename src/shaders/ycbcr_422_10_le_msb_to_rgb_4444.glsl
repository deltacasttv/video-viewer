constexpr char const * fragment_shader_ycbcr_422_10_le_msb_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform int texture_width;
uniform int texture_height;

// One texel = one little-endian 32-bit word (its 4 bytes in .rgba), one texture row = one video line.
// Every line starts on a new group of 6 pixels packed in 4 words (v210 order):
//   word 0 = Cb0 | Y0 << 10 | Cr0 << 20
//   word 1 = Y1  | Cb2 << 10 | Y2 << 20
//   word 2 = Cr2 | Y3 << 10 | Cb4 << 20
//   word 3 = Y4  | Cr4 << 10 | Y5 << 20
// Line padding (unused samples of the last group of a line, optional line alignment) is never read.
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

// Sample 'index' of the 6-pixel group starting at texel 'group_texel' of line 'line', normalized to [0,1].
// Samples are numbered in memory order: Cb0 Y0 Cr0 Y1 Cb2 Y2 Cr2 Y3 Cb4 Y4 Cr4 Y5 (3 per word, LSB first).
float group_sample(int group_texel, int index, int line)
{
    uvec4 bytes = uvec4(round(texelFetch(input_texture, ivec2(group_texel + index / 3, line), 0) * 255.0));
    uint word = bytes.r | (bytes.g << 8u) | (bytes.b << 16u) | (bytes.a << 24u);
    return float((word >> uint(10 * (index % 3))) & 0x3FFu) / 1023.0;
}

void main() {
    ivec2 coords = ivec2(floor(texture_coordinates * vec2(texture_width, texture_height)));

    int group_texel = (coords.x / 6) * 4;
    int phase = coords.x % 6;
    int cb_index = (phase / 2) * 4; // Cb of the pixel pair, Cr follows 2 samples later

    vec4 yuvk;
    yuvk.x = group_sample(group_texel, 2 * phase + 1, coords.y);
    yuvk.y = group_sample(group_texel, cb_index, coords.y);
    yuvk.z = group_sample(group_texel, cb_index + 2, coords.y);
    yuvk.w = 1.0;

    output_color = yuv2rgba(yuvk);
}
)";