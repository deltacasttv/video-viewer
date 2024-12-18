constexpr char const * fragment_shader_bgr_444_8_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform int texture_width;
uniform int texture_height;

uniform sampler2D input_texture;

void main() {
    vec4 bgr = texelFetch(input_texture, ivec2(round(texture_coordinates.x * texture_width), round(texture_coordinates.y * texture_height)), 0);
    output_color = vec4(bgr.b, bgr.g, bgr.r, 1.0);
}
)";