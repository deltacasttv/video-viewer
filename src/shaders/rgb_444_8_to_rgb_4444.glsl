constexpr char const * fragment_shader_rgb_444_8_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform sampler2D input_texture;

uniform int output_width;
uniform int output_height;

uniform bool bt_709;

void main() {
    vec3 rgb = texelFetch(input_texture, ivec2(round(texture_coordinates)), 0);
    output_color = vec4(rgb, 1.0);
}
)";