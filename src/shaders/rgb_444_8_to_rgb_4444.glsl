constexpr char const * fragment_shader_rgb_444_8_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform int texture_width;
uniform int texture_height;

uniform sampler2D input_texture;

void main() {
    // gl_FragCoord.xy is the centre (x + 0.5, y + 0.5) of the output pixel in the texture_width x texture_height
    // conversion viewport (bottom-left origin): truncating it gives the exact pixel with integer-only math on every
    // GPU (#42). y is flipped so that row 0 is the first line of the input buffer.
    ivec2 coords = ivec2(int(gl_FragCoord.x), texture_height - 1 - int(gl_FragCoord.y));
    vec3 rgb = texelFetch(input_texture, coords, 0).rgb;
    output_color = vec4(rgb, 1.0);
}
)";