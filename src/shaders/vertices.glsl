constexpr char const * vertex_shader_vertices = R"(#version 410

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 input_texture_coordinates;

out vec2 texture_coordinates;

void main() {
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    texture_coordinates = input_texture_coordinates;
}
)";