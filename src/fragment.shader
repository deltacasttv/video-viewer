constexpr char const * fragment_shader_src =
{
"#version 430\n"
"\n"
"in vec4 inColor;\n"
"in vec2 TexCoord;\n"
"out vec4 color;\n"
"\n"
"uniform sampler2D currentTexture;\n"
"\n"
"void main() {\n"
"	color = texture(currentTexture, TexCoord);\n"
"}\n"
}; 