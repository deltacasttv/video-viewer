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
    ivec2 pixel0 = ivec2(int(((output_normalised.x * output_texture_size.x) - mod(output_normalised.x * output_texture_size.x, 6.0)) * 2.0 / 3.0), int(output_normalised.y * output_texture_size.y));

    vec4 yuvk;
    vec4 uyvy;
    yuvk.w = 1.0;

   if(mod(output_pixel_index, 6.0) < 1.0)
   {
      uyvy = texelFetch(input_texture, pixel0, 0);
      yuvk.x = ((uyvy.y*255.0/4.0) + (mod(uyvy.z*255.0,16.0)*64.0))/1023.0;
      yuvk.y = (uyvy.x*255.0 + (mod(uyvy.y*255.0,4.0)*256.0))/1023.0;
      yuvk.z = ((uyvy.z*255.0/16.0) + (mod(uyvy.w*255.0,64.0)*16.0))/1023.0;
      
    output_color = yuv2rgba(yuvk);
   }
  //  else
  //  if (mod(output_pixel_index, 6.0) < 2.0)
  //  {
  //     uyvy = texelFetch(input_texture, pixel0, 0);
  //     yuvk.y = (uyvy.x*255.0 + (mod(uyvy.y*255.0,4.0)*256.0))/1023.0;
  //     yuvk.z = ((uyvy.z*255.0/16.0) + (mod(uyvy.w*255.0,64.0)*16.0))/1023.0;
  //     uyvy = texelFetch(input_texture, pixel0 + (1, 0) , 0);
  //     yuvk.x = (uyvy.x*255.0 + (mod(uyvy.y*255.0,4.0)*256.0))/1023.0;
  //     output_color = yuv2rgba(yuvk);
  //  }   
  //  else
  //  if (mod(output_pixel_index, 6.0) < 3.0)
  //  {
  //     uyvy = texelFetch(input_texture, pixel0 + (1,0), 0);
  //     yuvk.y = ((uyvy.y*255.0/4.0) + (mod(uyvy.z*255.0,16.0)*64.0))/1023.0;
  //     yuvk.x = ((uyvy.z*255.0/16.0) + (mod(uyvy.w*255.0,64.0)*16.0))/1023.0;
  //     uyvy = texelFetch(input_texture, pixel0 + (2, 0) , 0);
  //     yuvk.z = (uyvy.x*255.0 + (mod(uyvy.y*255.0,4.0)*256.0))/1023.0;
  //     output_color = yuv2rgba(yuvk);
  //  }
  //  else
  //  if (mod(output_pixel_index, 6.0) < 4.0)
  //  {
  //     uyvy = texelFetch(input_texture, pixel0 + (1,0), 0);
  //     yuvk.y = ((uyvy.y*255.0/4.0) + (mod(uyvy.z*255.0,16.0)*64.0))/1023.0;
  //     uyvy = texelFetch(input_texture, pixel0 + (2, 0) , 0);
  //     yuvk.z = (uyvy.x*255.0 + (mod(uyvy.y*255.0,4.0)*256.0))/1023.0;
  //     yuvk.x = ((uyvy.y*255.0/4.0) + (mod(uyvy.z*255.0,16.0)*64.0))/1023.0;
  //     output_color = yuv2rgba(yuvk);
  //  }
  //  else
  //  if (mod(output_pixel_index, 6.0) < 5.0)
  //  {
  //     uyvy = texelFetch(input_texture, pixel0 + (2,0), 0);
  //     yuvk.y = ((uyvy.z*255.0/16.0) + (mod(uyvy.w*255.0,64.0)*16.0))/1023.0;
  //     uyvy = texelFetch(input_texture, pixel0 + (3, 0) , 0);
  //     yuvk.x = (uyvy.x*255.0 + (mod(uyvy.y*255.0,4.0)*256.0))/1023.0;
  //     yuvk.z = ((uyvy.y*255.0/4.0) + (mod(uyvy.z*255.0,16.0)*64.0))/1023.0;
  //     output_color = yuv2rgba(yuvk);
  //  }
  //  else
  //  {
  //     uyvy = texelFetch(input_texture, pixel0 + (2,0), 0);
  //     yuvk.y = ((uyvy.z*255.0/16.0) + (mod(uyvy.w*255.0,64.0)*16.0))/1023.0;
  //     uyvy = texelFetch(input_texture, pixel0 + (3, 0) , 0);
  //     yuvk.x = ((uyvy.z*255.0/16.0) + (mod(uyvy.w*255.0,64.0)*16.0))/1023.0;
  //     yuvk.z = ((uyvy.y*255.0/4.0) + (mod(uyvy.z*255.0,16.0)*64.0))/1023.0;
  //     output_color = yuv2rgba(yuvk);
  //  }
}
)";