constexpr char const * fragment_shader_bgr_444_8_le_msb_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform sampler2D input_texture;

uniform int output_width;
uniform int output_height;

uniform bool bt_709;

void main() {
    vec3 bgr = texelFetch(input_texture, ivec2(texture_coordinates), 0);

    if(texture_coordinates > 1920/2)
    {
        output_color = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else
        output_color = vec4(bgr.b, bgr.g, bgr.r, 1.0);
}
)";