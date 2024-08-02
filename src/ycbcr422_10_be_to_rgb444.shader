constexpr char const *compute_shader_422_10_be = R"(

#version 430

layout(local_size_x = 8, local_size_y = 8) in;

layout(rgba8, binding = 0) uniform image2D img_output;
layout(rgba8, binding = 1) uniform image2D img_input;

vec4 yuv2rgba(vec4 yuvk)
{
    vec4 rgba;
    bool bt_709 = true; //TODO: pass this information from the code

    rgba.x = 1.164 * (yuvk.x - 16.0 / 255.0) + ((float(bt_709) * 1.793) + (float(!bt_709)) * (1.596)) * (yuvk.z - 128.0 / 255.0);
    rgba.y = 1.164 * (yuvk.x - 16.0 / 255.0) - ((float(bt_709) * 0.534) + (float(!bt_709)) * (0.813)) * (yuvk.z - 128.0 / 255.0) - 0.213 * (yuvk.y - 128.0 / 255.0);
    rgba.z = 1.164 * (yuvk.x - 16.0 / 255.0) + ((float(bt_709) * 2.115) + (float(!bt_709)) * (2.018)) * (yuvk.y - 128.0 / 255.0);
    rgba.w = 1.0;
    return rgba;
}

void main() {
    int width = imageSize(img_output).x;
    ivec2 coords = ivec2(gl_GlobalInvocationID);
    vec4 uyvy;
    vec4 uyvy2;
    float PxlIdx = coords.x + coords.y * width;
    vec4 yuvk;
    yuvk.w = 1.0;

   if (mod(PxlIdx, 8.0) < 1.0)
   {
     uyvy = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0), coords.y));
     yuvk.x = ((mod(uyvy.y * 255.0,64) * 16) + (int(uyvy.z * 255.0 / 16.0))) / 1023.0;
     yuvk.y = ((uyvy.x * 255.0 * 4.0) + (int(uyvy.y * 255.0 / 64.0))) / 1023.0;
     yuvk.z = ((mod(uyvy.z * 255.0,16.0) * 64.0) + (int(uyvy.w * 255.0 / 4.0))) / 1023.0;
   }
   else if (mod(PxlIdx, 8.0) < 2.0)
   {
     uyvy = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0), coords.y));
     yuvk.y = ((uyvy.x * 255.0 * 4.0) + (uyvy.y * 255.0 / 64.0)) / 1023.0;
     yuvk.z = ((mod(uyvy.z * 255.0,16.0) * 64.0) + (uyvy.w * 255.0 / 4.0)) / 1023.0;
     uyvy2 = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0) + 1, coords.y));
     yuvk.x = (uyvy2.x * 255.0 + (mod(uyvy.w * 255.0,4.0) * 256.0)) / 1023.0;
   }
   else if (mod(PxlIdx, 8.0) < 3.0)
   {
     uyvy = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0), coords.y));
     yuvk.x = ((mod(uyvy.z * 255.0, 64.0) * 16.0) + (uyvy.w * 255.0 / 16.0)) / 1023.0;
     yuvk.y = ((uyvy.y * 255.0 * 4.0) + (uyvy.z * 255.0 / 64.0)) / 1023.0;
     uyvy2 = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0) + 1, coords.y));
     yuvk.z = ((mod(uyvy.w * 255.0,16.0) * 64.0) + (uyvy2.x * 255.0 / 4.0)) / 1023.0;
   }
   else if (mod(PxlIdx, 8.0) < 4.0)
   {
     uyvy = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0) + 1, coords.y));
     yuvk.x = (uyvy.y * 255.0 + (mod(uyvy.x * 255.0,4.0) * 256.0)) / 1023.0;
     uyvy2 = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0), coords.y));
     yuvk.y = ((uyvy2.y * 255.0 * 4.0) + (uyvy2.z * 255.0 / 64.0)) / 1023.0;
     yuvk.z = ((mod(uyvy2.w * 255.0,16.0) * 64.0) + (uyvy.x * 255.0 / 4.0)) / 1023.0;
   }
   else if (mod(PxlIdx, 8.0) < 5.0)
   {
     uyvy = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0), coords.y));
     uyvy2 = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0) + 1 , coords.y));
     yuvk.x = ((mod(uyvy.w * 255.0, 64.0) * 16.0) + (uyvy2.x * 255.0 / 16.0)) / 1023.0;
     yuvk.y = ((uyvy.z * 255.0 * 4.0) + (uyvy.w * 255.0 / 64.0)) / 1023.0;
     yuvk.z = ((mod(uyvy2.x * 255.0,16.0) * 64.0) + (uyvy2.y * 255.0 / 4.0)) / 1023.0;
   }
   else if (mod(PxlIdx, 8.0) < 6.0)
   {
     uyvy = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0), coords.y));
     yuvk.x = (uyvy.z * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;
     uyvy2 = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0) - 1 , coords.y));
     yuvk.y = ((uyvy2.z * 255.0 * 4.0) + (uyvy2.w * 255.0 / 64.0)) / 1023.0;
     yuvk.z = ((mod(uyvy.x * 255.0,16.0) * 64.0) + (uyvy.y * 255.0 / 4.0)) / 1023.0;
   }
   else if (mod(PxlIdx, 8.0) < 7.0)
   {
     uyvy = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0), coords.y));
     uyvy2 = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0) + 1 , coords.y));
     yuvk.x = ((mod(uyvy2.x * 255.0, 64.0) * 16.0) + (uyvy2.y * 255.0 / 16.0)) / 1023.0;
     yuvk.y = ((uyvy.w * 255.0 * 4.0) + (uyvy2.x * 255.0 / 64.0)) / 1023.0;
     yuvk.z = ((mod(uyvy2.y * 255.0,16.0) * 64.0) + (uyvy2.z * 255.0 / 4.0)) / 1023.0;
   }
   else
   {
     uyvy = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0), coords.y));
     uyvy2 = imageLoad(img_input, ivec2(int(coords.x * 5.0 / 8.0) - 1, coords.y));
     yuvk.x = (uyvy.w * 255.0 + (mod(uyvy.z * 255.0,4.0) * 256.0)) / 1023.0;
     yuvk.y = ((uyvy2.w * 255.0 * 4.0) + (uyvy.x * 255.0 / 64.0)) / 1023.0;
     yuvk.z = ((mod(uyvy.y * 255.0,16.0) * 64.0) + (uyvy.z * 255.0 / 4.0)) / 1023.0;
   }

    imageStore(img_output, coords, yuv2rgba(yuvk));
}
)";