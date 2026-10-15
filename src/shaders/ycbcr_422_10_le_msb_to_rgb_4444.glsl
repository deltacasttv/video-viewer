constexpr char const * fragment_shader_ycbcr_422_10_le_msb_to_rgb_44444 = R"(#version 410

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

// Returns the 32-bit little-endian word number word_index of the v210 stream. The stream is one continuous sequence
// of 6-pixel groups over the whole frame, stored row by row in the RGBA8 input texture (one word per texel,
// r = least significant byte). The texel is clamped so that texelFetch is always in range.
uint read_word(int word_index)
{
    ivec2 size = textureSize(input_texture, 0);
    ivec2 texel = clamp(ivec2(word_index % size.x, word_index / size.x), ivec2(0), size - 1);
    // v * 255.0 is within rounding error of an integer: adding 0.5 and truncating gives the exact byte.
    uvec4 bytes = uvec4(texelFetch(input_texture, texel, 0) * 255.0 + 0.5);
    return bytes.r | (bytes.g << 8) | (bytes.b << 16) | (bytes.a << 24);
}

// Returns the normalized 10-bit sample number sample_index (0..11) of the group starting at word group_word.
// Samples are Cb0 Y0 Cr0 | Y1 Cb2 Y2 | Cr2 Y3 Cb4 | Y4 Cr4 Y5, three per word starting from the LSB.
float read_sample(int group_word, int sample_index)
{
    uint word = read_word(group_word + sample_index / 3);
    return float((word >> (10 * (sample_index % 3))) & 0x3FFu) / 1023.0;
}

void main() {
    // gl_FragCoord.xy is the centre (x + 0.5, y + 0.5) of the output pixel in the texture_width x texture_height
    // conversion viewport (bottom-left origin): truncating it gives the exact pixel with integer-only math on every
    // GPU (#42). y is flipped so that row 0 is the first line of the input buffer.
    ivec2 coords = ivec2(int(gl_FragCoord.x), texture_height - 1 - int(gl_FragCoord.y));
    int pixel_index = coords.y * texture_width + coords.x;
    int group_word = (pixel_index / 6) * 4;
    // Samples of a pixel pair follow each other in the group: Cb, Y of the even pixel, Cr, Y of the odd pixel.
    int pair_sample = ((pixel_index % 6) / 2) * 4;

    vec4 yuvk;
    yuvk.x = read_sample(group_word, pair_sample + 1 + 2 * (pixel_index & 1));
    yuvk.y = read_sample(group_word, pair_sample);
    yuvk.z = read_sample(group_word, pair_sample + 2);
    yuvk.w = 1.0;

    output_color = yuv2rgba(yuvk);
}
)";