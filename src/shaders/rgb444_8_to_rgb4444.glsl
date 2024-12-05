constexpr char const * fragment_shader_rgb444_8_to_rgb44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform sampler2D input_texture;

void main() {
    vec3 rgb = texture(input_texture, texture_coordinates).rgb;
	output_color = vec4(rgb, 1.0);
}
)";