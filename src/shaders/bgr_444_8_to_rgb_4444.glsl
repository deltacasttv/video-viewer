constexpr char const * fragment_shader_bgr_444_8_le_msb_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform sampler2D input_texture;

uniform int output_width;
uniform int output_height;

uniform bool bt_709;

void main() {
    vec3 rgb = texture(input_texture, texture_coordinates).rgb;
    output_color = vec4(rgb.b, rgb.g, rgb.r, 1.0);
}
)";