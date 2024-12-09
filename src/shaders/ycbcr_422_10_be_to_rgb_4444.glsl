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

    vec4 yuvk;
    yuvk.w = 1.0;
    if(mod(pixel_index, 8) == 0)
    {
        vec4 uyvy = texelFetch(input_texture, texture_coords, 0);
        vec4 uyvy_next = texelFetch(input_texture, texture_coords + ivec2(1, 0), 0);

        int u = (int(uyvy.r * 255.0) << 2) | ((int(uyvy.g * 255.0) >> 6) & 0x03);
        // int y0 = ((int(uyvy.g * 255.0) & 0x3F) << 4) | ((int(uyvy.b * 255.0) >> 4) & 0x0F);
        // int v = ((int(uyvy.b * 255.0) & 0x0F) << 6) | ((int(uyvy.a * 255.0) >> 2) & 0x3F);
        // int y1 = ((int(uyvy_next.r * 255.0) & 0x03) << 8) | (int(uyvy_next.g * 255.0));

        // // Normalisation
        float U = float(u) / 1023.0;
        // float Y0 = float(y0) / 1023.0;
        // float V = float(v) / 1023.0;
        // float Y1 = float(y1) / 1023.0;

        float tolerance = 0.05; // Marge de 5%
        int expected_u = 0x80;  // 128 sur 10 bits
        float relative_error = abs(float(u) - float(expected_u)) / float(expected_u);

        if (relative_error <= tolerance) {
            // Le résultat est dans la marge d'erreur
            output_color = vec4(0.0, 1.0, 0.0, 1.0); // Vert = OK
        } else {
            // Résultat hors tolérance
            output_color = vec4(float(u) / 1023.0, 0.0, 0.0, 1.0); // Rouge = Erreur
        }   



        // yuvk = vec4(Y0, U, V, 1.0);

        // output_color = yuv2rgba(yuvk, bt_709);
    }
    // else if(mod(pixel_index, 8.0) < 2.0)
    // {
    //     vec4 uyvy = texelFetch(input_texture, texture_coords, 0);
    //     vec4 uyvy_next = texelFetch(input_texture, texture_coords + ivec2(1, 0), 0);
    //     // yuvk.y = ((uyvy.x * 255.0 * 4.0) + (uyvy.y * 255.0 / 64.0)) / 1023.0;
    //     // yuvk.z = ((mod(uyvy.z * 255.0,16.0) * 64.0) + (uyvy.w * 255.0 / 4.0)) / 1023.0;
    //     yuvk.x = (uyvy_next.x * 255.0 + (mod(uyvy.w * 255.0,4.0) * 256.0)) / 1023.0;
    // }
    // else if(mod(pixel_index, 8.0) < 3.0)
    // {
    //     vec4 uyvy = texelFetch(input_texture, texture_coords, 0);
    //     vec4 uyvy_next = texelFetch(input_texture, texture_coords + ivec2(1, 0), 0);
    //  yuvk.x = ((mod(uyvy.z * 255.0, 64.0) * 16.0) + (uyvy.w * 255.0 / 16.0)) / 1023.0;
    // //  yuvk.y = ((uyvy.y * 255.0 * 4.0) + (uyvy.z * 255.0 / 64.0)) / 1023.0;
    // //  yuvk.z = ((mod(uyvy.w * 255.0,16.0) * 64.0) + (uyvy_next.x * 255.0 / 4.0)) / 1023.0;
    // }
    // else if(mod(pixel_index, 8.0) < 4.0)
    // {
    //     vec4 uyvy = texelFetch(input_texture, texture_coords, 0);
    //     vec4 uyvy_next = texelFetch(input_texture, texture_coords + ivec2(1, 0), 0);
    //  yuvk.x = (uyvy.y * 255.0 + (mod(uyvy.x * 255.0,4.0) * 256.0)) / 1023.0;
    // //  yuvk.y = ((uyvy_next.y * 255.0 * 4.0) + (uyvy_next.z * 255.0 / 64.0)) / 1023.0;
    // //  yuvk.z = ((mod(uyvy_next.w * 255.0,16.0) * 64.0) + (uyvy.x * 255.0 / 4.0)) / 1023.0;
    // }
    // else if(mod(pixel_index, 8.0) < 5.0)
    // {
    //     vec4 uyvy = texelFetch(input_texture, texture_coords, 0);
    //     vec4 uyvy_next = texelFetch(input_texture, texture_coords + ivec2(1, 0), 0);
    //  yuvk.x = ((mod(uyvy.w * 255.0, 64.0) * 16.0) + (uyvy_next.x * 255.0 / 16.0)) / 1023.0;
    // //  yuvk.y = ((uyvy.z * 255.0 * 4.0) + (uyvy.w * 255.0 / 64.0)) / 1023.0;
    // //  yuvk.z = ((mod(uyvy_next.x * 255.0,16.0) * 64.0) + (uyvy_next.y * 255.0 / 4.0)) / 1023.0;
    // }
    // else if(mod(pixel_index, 8.0) < 6.0)
    // {
    //     vec4 uyvy = texelFetch(input_texture, texture_coords, 0);
    //     vec4 uyvy_next = texelFetch(input_texture, texture_coords + ivec2(1, 0), 0);
    //  yuvk.x = (uyvy.z * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;
    // //  yuvk.y = ((uyvy_next.z * 255.0 * 4.0) + (uyvy_next.w * 255.0 / 64.0)) / 1023.0;
    // //  yuvk.z = ((mod(uyvy.x * 255.0,16.0) * 64.0) + (uyvy.y * 255.0 / 4.0)) / 1023.0;
    // }
    // else if(mod(pixel_index, 8.0) < 7.0)
    // {
    //     vec4 uyvy = texelFetch(input_texture, texture_coords, 0);
    //     vec4 uyvy_next = texelFetch(input_texture, texture_coords + ivec2(1, 0), 0);
    //  yuvk.x = ((mod(uyvy_next.x * 255.0, 64.0) * 16.0) + (uyvy_next.y * 255.0 / 16.0)) / 1023.0;
    // //  yuvk.y = ((uyvy.w * 255.0 * 4.0) + (uyvy_next.x * 255.0 / 64.0)) / 1023.0;
    // //  yuvk.z = ((mod(uyvy_next.y * 255.0,16.0) * 64.0) + (uyvy_next.z * 255.0 / 4.0)) / 1023.0;
    // }
    // else if(mod(pixel_index, 8.0) < 8.0)
    // {
    //     vec4 uyvy = texelFetch(input_texture, texture_coords, 0);
    //     vec4 uyvy_next = texelFetch(input_texture, texture_coords + ivec2(1, 0), 0);
    //  yuvk.x = (uyvy.w * 255.0 + (mod(uyvy.z * 255.0,4.0) * 256.0)) / 1023.0;
    // //  yuvk.y = ((uyvy_next.w * 255.0 * 4.0) + (uyvy.x * 255.0 / 64.0)) / 1023.0;
    // //  yuvk.z = ((mod(uyvy.y * 255.0,16.0) * 64.0) + (uyvy.z * 255.0 / 4.0)) / 1023.0;
    // }
    else
    {
        output_color = vec4(0.0, 0.0, 0.0, 1.0);
    }


}
)";