constexpr char const * fragment_shader_ycbcr_422_10_be_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform sampler2D input_texture;

vec4 yuv2rgba(vec4 yuvk, bool bt_709)
{
    vec4 rgba;

    rgba.x = 1.164 * (yuvk.x - (16.0 / 255.0)) + ((float(bt_709) * 1.793) + (float(!bt_709)) * (1.596)) * (yuvk.z - (128.0 / 255.0));
    rgba.y = 1.164 * (yuvk.x - (16.0 / 255.0)) - ((float(bt_709) * 0.534) + (float(!bt_709)) * (0.813)) * (yuvk.z - (128.0 / 255.0)) - 0.213 * (yuvk.y - (128.0 / 255.0));
    rgba.z = 1.164 * (yuvk.x - (16.0 / 255.0)) + ((float(bt_709) * 2.115) + (float(!bt_709)) * (2.018)) * (yuvk.y - (128.0 / 255.0));
    rgba.w = 1.0;
    return rgba;
}

void main() {
    bool bt_709 = true; //TODO: pass this information from the code
    ivec2 texture_size = textureSize(input_texture, 0);
    ivec2 texture_coords = ivec2(floor(texture_size.x * texture_coordinates.x), floor(texture_size.y * texture_coordinates.y));
    int pixel_index = int(texture_coords.x + texture_coords.y * texture_size.x);
    
    ivec2 pixel_coords = ivec2(gl_FragCoord.x * texture_size.x, gl_FragCoord.y * texture_size.y);
    int out_pixel_index = int(pixel_coords.x + pixel_coords.y * texture_size.x);

    vec4 yuvk;
    yuvk.w = 1.0;
    if(mod(pixel_index, 5) == 0)
    {
        vec4 uyvy = texelFetch(input_texture, texture_coords, 0);
        vec4 uyvy_next = texelFetch(input_texture, texture_coords + ivec2(1, 0), 0);

        float u = float((int(uyvy.r * 255.0) << 2) | ((int(uyvy.g * 255.0) >> 6) & 0x03)) / 1023.0;
        float y0 = float(((int(uyvy.g * 255.0) & 0x3F) << 4) | ((int(uyvy.b * 255.0) >> 4) & 0x0F)) / 1023.0;
        float v = float(((int(uyvy.b * 255.0) & 0x0F) << 6) | ((int(uyvy.a * 255.0) >> 2) & 0x3F)) / 1023.0;
        float y1 = float(((int(uyvy.a * 255.0) & 0xC0) << 8) | (int(uyvy_next.r * 255.0))) / 1023.0;

        if(out_pixel_index % 2 == 0)
        {
            yuvk = vec4(y0, u, v, 1.0);
        }
        else
        {
            yuvk = vec4(y1, u, v, 1.0);
        }
    }
    else if(mod(pixel_index, 5) == 1)
    {
        vec4 uyvy = texelFetch(input_texture, texture_coords, 0);
        vec4 uyvy_next = texelFetch(input_texture, texture_coords + ivec2(1, 0), 0);

        float u = float((int(uyvy.g * 255.0) << 2) | ((int(uyvy.b * 255.0) >> 6) & 0x03)) / 1023.0;
        float y0 = float(((int(uyvy.b * 255.0) & 0x3F) << 4) | ((int(uyvy.a * 255.0) >> 4) & 0x0F)) / 1023.0;
        float v = float(((int(uyvy.a * 255.0) & 0x0F) << 6) | ((int(uyvy_next.r * 255.0) >> 2) & 0x3F)) / 1023.0;
        float y1 = float(((int(uyvy_next.r * 255.0) & 0xC0) << 8) | (int(uyvy_next.g * 255.0))) / 1023.0;

        if(out_pixel_index % 2 == 0)
        {
            yuvk = vec4(y0, u, v, 1.0);
        }
        else
        {
            yuvk = vec4(y1, u, v, 1.0);
        }
    }
    else if(mod(pixel_index, 5) == 2)
    {
        vec4 uyvy = texelFetch(input_texture, texture_coords, 0);
        vec4 uyvy_next = texelFetch(input_texture, texture_coords + ivec2(1, 0), 0);

        float u = float((int(uyvy.b * 255.0) << 2) | ((int(uyvy.a * 255.0) >> 6) & 0x03)) / 1023.0;
        float y0 = float(((int(uyvy.a * 255.0) & 0x3F) << 4) | ((int(uyvy_next.r * 255.0) >> 4) & 0x0F)) / 1023.0;
        float v = float(((int(uyvy_next.r * 255.0) & 0x0F) << 6) | ((int(uyvy_next.g * 255.0) >> 2) & 0x3F)) / 1023.0;
        float y1 = float(((int(uyvy_next.g * 255.0) & 0xC0) << 8) | (int(uyvy_next.b * 255.0))) / 1023.0;

        if(out_pixel_index % 2 == 0)
        {
            yuvk = vec4(y0, u, v, 1.0);
        }
        else
        {
            yuvk = vec4(y1, u, v, 1.0);
        }
    }
    else if(mod(pixel_index, 5) == 3)
    {
        vec4 uyvy = texelFetch(input_texture, texture_coords, 0);
        vec4 uyvy_next = texelFetch(input_texture, texture_coords + ivec2(1, 0), 0);

        float u = float((int(uyvy.a * 255.0) << 2) | ((int(uyvy_next.r * 255.0) >> 6) & 0x03)) / 1023.0;
        float y0 = float(((int(uyvy_next.r * 255.0) & 0x3F) << 4) | ((int(uyvy_next.g * 255.0) >> 4) & 0x0F)) / 1023.0;
        float v = float(((int(uyvy_next.g * 255.0) & 0x0F) << 6) | ((int(uyvy_next.b * 255.0) >> 2) & 0x3F)) / 1023.0;
        float y1 = float(((int(uyvy_next.b * 255.0) & 0xC0) << 8) | (int(uyvy_next.a * 255.0))) / 1023.0;

        if(out_pixel_index % 2 == 0)
        {
            yuvk = vec4(y0, u, v, 1.0);
        }
        else
        {
            yuvk = vec4(y1, u, v, 1.0);
        }
    }
    else if(mod(pixel_index, 5) == 4)
    {
        vec4 uyvy = texelFetch(input_texture, texture_coords, 0);
        vec4 uyvy_next = texelFetch(input_texture, texture_coords + ivec2(1, 0), 0);
        vec4 uyvy_next2 = texelFetch(input_texture, texture_coords + ivec2(2, 0), 0);

        float u = float((int(uyvy_next.r * 255.0) << 2) | ((int(uyvy_next.g * 255.0) >> 6) & 0x03)) / 1023.0;
        float y0 = float(((int(uyvy_next.g * 255.0) & 0x3F) << 4) | ((int(uyvy_next.b * 255.0) >> 4) & 0x0F)) / 1023.0;
        float v = float(((int(uyvy_next.b * 255.0) & 0x0F) << 6) | ((int(uyvy_next.a * 255.0) >> 2) & 0x3F)) / 1023.0;
        float y1 = float(((int(uyvy_next.a * 255.0) & 0xC0) << 8) | (int(uyvy_next2.r * 255.0))) / 1023.0;

        if(out_pixel_index % 2 == 0)
        {
            yuvk = vec4(y0, u, v, 1.0);
        }
        else
        {
            yuvk = vec4(y1, u, v, 1.0);
        }
    }
    
    output_color = yuv2rgba(yuvk, bt_709);
}
)";