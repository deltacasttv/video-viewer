constexpr char const * fragment_shader_render = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform sampler2D input_texture;

void main() {
	output_color = texture(input_texture, texture_coordinates).rgba;
}
)";