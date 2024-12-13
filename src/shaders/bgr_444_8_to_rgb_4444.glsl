constexpr char const * fragment_shader_bgr_444_8_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform sampler2D input_texture;

void main() {
    vec4 bgr = texelFetch(input_texture, ivec2(texture_coordinates), 0);
    output_color = vec4(bgr.b, bgr.g, bgr.r, 1.0);
}
)";