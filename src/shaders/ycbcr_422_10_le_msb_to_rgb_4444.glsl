constexpr char const * fragment_shader_ycbcr_422_10_le_msb_to_rgb_44444 = R"(#version 410

in vec2 texture_coordinates;
out vec4 output_color;

uniform sampler2D input_texture;

uniform int output_width;
uniform int output_height;

uniform bool bt_709;

vec4 yuv2rgba(vec4 yuvk)
{
    vec4 rgba;

    rgba.x = 1.164 * (yuvk.x - (16.0 / 255.0)) + ((float(bt_709) * 1.793) + (float(!bt_709)) * (1.596)) * (yuvk.z - (128.0 / 255.0));
    rgba.y = 1.164 * (yuvk.x - (16.0 / 255.0)) - ((float(bt_709) * 0.534) + (float(!bt_709)) * (0.813)) * (yuvk.z - (128.0 / 255.0)) - 0.213 * (yuvk.y - (128.0 / 255.0));
    rgba.z = 1.164 * (yuvk.x - (16.0 / 255.0)) + ((float(bt_709) * 2.115) + (float(!bt_709)) * (2.018)) * (yuvk.y - (128.0 / 255.0));
    rgba.w = 1.0;
    return rgba;
}

void main() {
    ivec2 input_texture_size = textureSize(input_texture, 0);
    ivec2 output_texture_size = ivec2(input_texture_size.x * 3.0 / 2.0, input_texture_size.y);
    ivec2 window_size = ivec2(output_width, output_height);
    vec2 output_normalised = vec2(gl_FragCoord.x / window_size.x, gl_FragCoord.y / window_size.y);
    // ivec2 output_pixel = ivec2(output_normalised.x * output_texture_size.x, output_normalised.y * output_texture_size.y);
    float output_pixel_index = output_normalised.x * output_texture_size.x + output_normalised.y * output_texture_size.y * output_texture_size.x;

    vec4 yuvk;
    vec4 uyvy;
    yuvk.w = 1.0;

   if(mod(output_pixel_index, 6.0) < 1.0)
   {
      // uyvy = texture(input_texture, vec2(output_normalised.x * 2.0 / 3.0, output_normalised.y), 0);
      uyvy = texelFetch(input_texture, ivec2(int(int(output_normalised.x * output_texture_size.x) * 2 / 3), int(output_normalised.y * output_texture_size.y)), 0);
      yuvk.x = ((uyvy.y*255.0/4.0) + (mod(uyvy.z*255.0,16.0)*64.0))/1023.0;
      yuvk.y = (uyvy.x*255.0 + (mod(uyvy.y*255.0,4.0)*256.0))/1023.0;
      yuvk.z = ((uyvy.z*255.0/16.0) + (mod(uyvy.w*255.0,64.0)*16.0))/1023.0;
      
    output_color = yuv2rgba(yuvk);
    // output_color = vec4(1.0, 0.0, 0.0, 1.0);
   }
  //  else if (mod(output_pixel_index, 6.0) < 2.0)
  //  {
  //     uyvy = texelFetch(input_texture, ivec2(floor((output_normalised.x * output_texture_size.x * 2.0) / 3.0), floor(output_normalised.y * output_texture_size.y)), 0);
  //   //   yuvk.y = float((int(uyvy.x * 255.0) & 0xFF) >> 0 | (int(uyvy.y * 255.0) & 0x03) << 8) / 1023.0;
  //   //   yuvk.x = float((int(uyvy.y * 255.0) & 0xFC) >> 2 | (int(uyvy.z * 255.0) & 0x0F) << 6) / 1023.0;
  //   //   yuvk.z = float((int(uyvy.z * 255.0) & 0xF0) >> 4 | (int(uyvy.w * 255.0) & 0x3F) << 4) / 1023.0;
  //     yuvk.y = (uyvy.x*255.0 + (mod(uyvy.y*255.0,4.0)*256.0))/1023.0;
  //     yuvk.z = ((uyvy.z*255.0/16.0) + (mod(uyvy.w*255.0,64.0)*16.0))/1023.0;
  //    uyvy = texture(input_texture, vec2(x, y) / output_texture_size, 0);
  //    yuvk.x = (uyvy.x*255.0 + (mod(uyvy.y*255.0,4.0)*256.0))/1023.0;
    
      
  //   output_color = yuv2rgba(yuvk);
  //  }
//    else if(mod(pixel_index, 6.0)< 2.0)
//    {
//     //  uyvy = texture(input_texture, vec2(xf-(2.0/3.0), yf));
//      uyvy = texelFetch(input_texture, ivec2(floor(xf - 1.0), floor(yf)), 0);
//     //  yuvk.y = float((int(uyvy.x * 255.0) & 0xFF) >> 0 | (int(uyvy.y * 255.0) & 0x03) << 8) / 1023.0;
//     //  yuvk.z = float((int(uyvy.z * 255.0) & 0xF0) >> 4 | (int(uyvy.w * 255.0) & 0x3F) << 4) / 1023.0;
//       yuvk.y = (uyvy.x*255.0 + (mod(uyvy.y*255.0,4.0)*256.0))/1023.0;
//       yuvk.z = ((uyvy.z*255.0/16.0) + (mod(uyvy.w*255.0,64.0)*16.0))/1023.0;

//     //  uyvy = texture(input_texture, vec2(xf+(2.0/3.0), yf));
//      uyvy = texelFetch(input_texture, ivec2(floor(xf + 1.0), floor(yf)), 0);
//     //  yuvk.x = float((int(uyvy.x * 255.0) & 0xFF) >> 0 | (int(uyvy.y * 255.0) & 0x03) << 8) / 1023.0;
//       yuvk.x = (uyvy.x*255.0 + (mod(uyvy.y*255.0,4.0)*256.0))/1023.0;

//    }
//    else if(mod(pixel_index, 6.0)< 3.0)
//    {
//     //  uyvy = texture(input_texture, vec2(xf, yf));
//      uyvy = texelFetch(input_texture, ivec2(floor(xf), floor(yf)), 0);
//       yuvk.x = ((uyvy.z*255.0/16.0) + (mod(uyvy.w*255.0,64.0)*16.0))/1023.0;
//       yuvk.y = ((uyvy.y*255.0/4.0) + (mod(uyvy.z*255.0,16.0)*64.0))/1023.0;
//     //  uyvy = texture(input_texture, vec2(xf+(2.0/3.0), yf));
//      uyvy = texelFetch(input_texture, ivec2(floor(xf + 1.0), floor(yf)), 0);
//       yuvk.z = (uyvy.x*255.0 + (mod(uyvy.y*255.0,4.0)*256.0))/1023.0;
//    }
//    else if(mod(pixel_index, 6.0)< 4.0)
//    {
//     //  uyvy = texture(input_texture, vec2(xf, yf));
//      uyvy = texelFetch(input_texture, ivec2(floor(xf), floor(yf)), 0);
//       yuvk.x = ((uyvy.y*255.0/4.0) + (mod(uyvy.z*255.0,16.0)*64.0))/1023.0;
//       yuvk.z = (uyvy.x*255.0 + (mod(uyvy.y*255.0,4.0)*256.0))/1023.0;
//     //   uyvy = texture(input_texture, vec2(xf-(2.0/3.0), yf));
//      uyvy = texelFetch(input_texture, ivec2(floor(xf - 1.0), floor(yf)), 0);
//       yuvk.y = ((uyvy.y*255.0/4.0) + (mod(uyvy.z*255.0,16.0)*64.0))/1023.0;
//    }
//    else if(mod(pixel_index, 6.0)< 5.0)
//    {
//     //   uyvy = texture(input_texture, vec2(xf-(2.0/3.0), yf));
//      uyvy = texelFetch(input_texture, ivec2(floor(xf - 1.0), floor(yf)), 0);
//      yuvk.y = ((uyvy.z*255.0/16.0) + (mod(uyvy.w*255.0,64.0)*16.0))/1023.0;
//     //   uyvy = texture(input_texture, vec2(xf+(2.0/3.0), yf));
//      uyvy = texelFetch(input_texture, ivec2(floor(xf + 1.0), floor(yf)), 0);
//       yuvk.x = (uyvy.x*255.0 + (mod(uyvy.y*255.0,4.0)*256.0))/1023.0;
//       yuvk.z = ((uyvy.y*255.0/4.0) + (mod(uyvy.z*255.0,16.0)*64.0))/1023.0;
//    }
//    else
//    {
//     //  uyvy = texture(input_texture, vec2(xf, yf));
//      uyvy = texelFetch(input_texture, ivec2(floor(xf), floor(yf)), 0);
//       yuvk.x = ((uyvy.z*255.0/16.0) + (mod(uyvy.w*255.0,64.0)*16.0))/1023.0;
//       yuvk.z = ((uyvy.y*255.0/4.0) + (mod(uyvy.z*255.0,16.0)*64.0))/1023.0;
//     //  uyvy = texture(input_texture, vec2(xf-(4.0/3.0), yf));
//      uyvy = texelFetch(input_texture, ivec2(floor(xf - 2.0), floor(yf)), 0);
//       yuvk.y = ((uyvy.z*255.0/16.0) + (mod(uyvy.w*255.0,64.0)*16.0))/1023.0;
//    }
    else
    {
      output_color = vec4(0.0, 0.0, 0.0, 1.0);
    }



}
)";