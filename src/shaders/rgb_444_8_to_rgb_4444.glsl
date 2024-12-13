constexpr char const * fragment_shader_rgb_444_8_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform sampler2D input_texture;

void main() {
    vec3 rgb = texelFetch(input_texture, ivec2(round(texture_coordinates)), 0).rgb;
    output_color = vec4(rgb, 1.0);
}
)";