constexpr char const * fragment_shader_bgr_444_8_le_msb_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform sampler2D input_texture;

uniform int output_width;
uniform int output_height;

uniform bool bt_709;

void main() {
    vec4 bgra = texelFetch(input_texture, ivec2(round(texture_coordinates)), 0);
    output_color = vec4(bgra.z, bgra.y, bgra.x, 1.0);
}
)";