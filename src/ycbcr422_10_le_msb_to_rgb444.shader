constexpr char const *compute_shader_422_10_le_msb =
{
"#version 430\n"
"\n"
"layout(local_size_x = 8, local_size_y = 8) in;\n"
"\n"
"layout(rgba8, binding = 0) uniform image2D img_output;\n"
"layout(rgba8, binding = 1) uniform image2D img_input;\n"
"\n"
"vec4 yuv2rgba(vec4 yuvk)\n"
"{\n"
"    vec4 rgba;\n"
"    bool bt_709 = true; //TODO: pass this information from the code\n"
"\n"
"    rgba.x = 1.164 * (yuvk.x - 16.0 / 255.0) + ((float(bt_709) * 1.793) + (float(!bt_709)) * (1.596)) * (yuvk.z - 128.0 / 255.0);\n"
"    rgba.y = 1.164 * (yuvk.x - 16.0 / 255.0) - ((float(bt_709) * 0.534) + (float(!bt_709)) * (0.813)) * (yuvk.z - 128.0 / 255.0) - 0.213 * (yuvk.y - 128.0 / 255.0);\n"
"    rgba.z = 1.164 * (yuvk.x - 16.0 / 255.0) + ((float(bt_709) * 2.115) + (float(!bt_709)) * (2.018)) * (yuvk.y - 128.0 / 255.0);\n"
"    rgba.w = 1.0;\n"
"    return rgba;\n"
"}\n"
"\n"
"void main() {\n"
"    int width = imageSize(img_output).x;\n"
"    ivec2 coords = ivec2(gl_GlobalInvocationID);\n"
"    vec4 uyvy;\n"
"    float PxlIdx = coords.x + coords.y * width;\n"
"    vec4 yuvk;\n"
"    yuvk.w = 1.0;\n"
"\n"
"   if (mod(PxlIdx, 6.0) < 1.0)\n"
"   {\n"
"     uyvy = imageLoad(img_input, ivec2(int((coords.x + mod(coords.y,3.0)*width)*2.0/3.0), coords.y / 3));\n"
"     yuvk.x = ((uyvy.y * 255.0 / 4.0) + (mod(uyvy.z * 255.0,16.0) * 64.0)) / 1023.0;\n"
"     yuvk.y = (uyvy.x * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;\n"
"     yuvk.z = ((uyvy.z * 255.0 / 16.0) + (mod(uyvy.w * 255.0,64.0) * 16.0)) / 1023.0;\n"
"   }\n"
"   else if (mod(PxlIdx, 6.0) < 2.0)\n"
"   {\n"
"     uyvy = imageLoad(img_input, ivec2(int((coords.x + mod(coords.y,3.0)*width)*2.0/3.0), coords.y / 3.0));\n"
"     yuvk.y = (uyvy.x * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;\n"
"     yuvk.z = ((uyvy.z * 255.0 / 16.0) + (mod(uyvy.w * 255.0,64.0) * 16.0)) / 1023.0;\n"
"     uyvy = imageLoad(img_input, ivec2(int((coords.x + mod(coords.y,3.0)*width)*2.0/3.0) + 1, coords.y / 3.0));\n"
"     yuvk.x = (uyvy.x * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;\n"
"   }\n"
"   else if (mod(PxlIdx, 6.0) < 3.0)\n"
"   {\n"
"     uyvy = imageLoad(img_input, ivec2(int((coords.x + mod(coords.y,3.0)*width)*2.0/3.0), coords.y / 3.0));\n"
"     yuvk.x = ((uyvy.z * 255.0 / 16.0) + (mod(uyvy.w * 255.0,64.0) * 16.0)) / 1023.0;\n"
"     yuvk.y = ((uyvy.y * 255.0 / 4.0) + (mod(uyvy.z * 255.0,16.0) * 64.0)) / 1023.0;\n"
"     uyvy = imageLoad(img_input, ivec2(int((coords.x + mod(coords.y,3.0)*width)*2.0/3.0)+1, coords.y / 3.0));\n"
"     yuvk.z = (uyvy.x * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;\n"
"   }\n"
"   else if (mod(PxlIdx, 6.0) < 4.0)\n"
"   {\n"
"     uyvy = imageLoad(img_input, ivec2(int((coords.x + mod(coords.y,3.0)*width)*2.0/3.0), coords.y / 3.0));\n"
"     yuvk.x = ((uyvy.y * 255.0 / 4.0) + (mod(uyvy.z * 255.0,16.0) * 64.0)) / 1023.0;\n"
"     yuvk.z = (uyvy.x * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;\n"
"     uyvy = imageLoad(img_input, ivec2(int((coords.x + mod(coords.y,3.0)*width)*2.0/3.0)-1, coords.y / 3.0));\n"
"     yuvk.y = ((uyvy.y * 255.0 / 4.0) + (mod(uyvy.z * 255.0,16.0) * 64.0)) / 1023.0;\n"
"   }\n"
"   else if (mod(PxlIdx, 6.0) < 5.0)\n"
"   {\n"
"     uyvy = imageLoad(img_input, ivec2(int((coords.x + mod(coords.y,3.0)*width)*2.0/3.0), coords.y / 3.0));\n"
"     yuvk.y = ((uyvy.z * 255.0 / 16.0) + (mod(uyvy.w * 255.0,64.0) * 16.0)) / 1023.0;\n"
"     uyvy = imageLoad(img_input, ivec2(int((coords.x + mod(coords.y,3.0)*width)*2.0/3.0)+1 , coords.y / 3.0));\n"
"     yuvk.x = (uyvy.x * 255.0 + (mod(uyvy.y * 255.0,4.0) * 256.0)) / 1023.0;\n"
"     yuvk.z = ((uyvy.y * 255.0 / 4.0) + (mod(uyvy.z * 255.0,16.0) * 64.0)) / 1023.0;\n"
"   }\n"
"   else\n"
"   {\n"
"     uyvy = imageLoad(img_input, ivec2(int((coords.x + mod(coords.y,3.0)*width)*2.0/3.0), coords.y / 3.0));\n"
"     yuvk.x = ((uyvy.z * 255.0 / 16.0) + (mod(uyvy.w * 255.0,64.0) * 16.0)) / 1023.0;\n"
"     yuvk.z = ((uyvy.y * 255.0 / 4.0) + (mod(uyvy.z * 255.0,16.0) * 64.0)) / 1023.0;\n"
"     uyvy = imageLoad(img_input, ivec2(int((coords.x + mod(coords.y,3.0)*width)*2.0/3.0)-1, coords.y / 3.0));\n"
"     yuvk.y = ((uyvy.z * 255.0 / 16.0) + (mod(uyvy.w * 255.0,64.0) * 16.0)) / 1023.0;\n"
"   }\n"
"\n"
"    imageStore(img_output, coords, yuv2rgba(yuvk));\n"
"}\n"
}; 