constexpr char const * vertex_shader_src =
{
"#version 430\n"
"\n"
"in vec3 vertex;\n"
"layout(location = 0) in vec2 position;\n"
"layout(location = 1) in vec2 texCoord;\n"
"\n"
"out vec4 inColor;\n"
"out vec2 TexCoord;\n"
"\n"
"void main() {\n"
"	gl_Position = vec4(position, .5f, 1.0f);\n"
"	inColor = gl_Position;\n"
"	TexCoord = texCoord;\n"
"}\n"
}; 