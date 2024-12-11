constexpr char const * fragment_shader_bgr_444_8_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform sampler2D input_texture;

uniform int output_width;
uniform int output_height;

uniform bool bt_709;

void main() {
    vec4 bgra = texture(input_texture, texture_coordinates);
    output_color = vec4(bgra.z, bgra.y, bgra.x, 1.0);
}
)";