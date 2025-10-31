/*
 * SPDX-FileCopyrightText: Copyright (c) DELTACAST.TV. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at * * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fstream>
#include <iostream>
#include "colorbar.hpp"

using namespace Deltacast;

// This structure is used to store the YUV 4:2:2 10bit little endian color bar pattern in memory
typedef struct
{
   uint32_t part_0;
   uint32_t part_1;
   uint32_t part_2;
   uint32_t part_3;
}DataV210;


// This structure is used to store the YUV 4:2:2 10bit Big Endian color bar pattern in memory
typedef struct
{
   uint8_t part_0;
   uint8_t part_1;
   uint8_t part_2;
   uint8_t part_3;
   uint8_t part_4;
}DataYUV42210BE;


#define V208_WHITE75     0xB480B480
#define V208_YELLOW75    0xA888A82C
#define V208_CYAN75      0x912C9193
#define V208_GREEN75     0x8534853F
#define V208_MAGENTA75   0x3FCC3FC1
#define V208_RED75       0x33D4336D
#define V208_BLUE75      0x1C781CD4
#define V208_BLACK75     0x10801080
#define V208_MINUSI      0x3D633D99
#define V208_PLUSQ       0x239823AE
#define V208_WHITE100    0xEB80EB80
#define V208_BLACKMINUS2 0x0C800C80
#define V208_BLACKPLUS2  0x14801480

#define V210_HD_WHITE75   {0x200B4200,0x2D0802D0,0x200B4200,0x2D0802D0} /* 6 pixels */
#define V210_HD_YELLOW75  {0x220A80B0,0x2A02C2A0,0x0B0A8220,0x2A0882A0} /* 6 pixels */
#define V210_HD_CYAN75    {0x0B09124C,0x24493244,0x24C910B0,0x2442C244} /* 6 pixels */
#define V210_HD_GREEN75   {0x0D0850FC,0x2143D214,0x0FC850D0,0x21434214}  /* 6 pixels */
#define V210_HD_MAGENTA75 {0x3303F304,0x0FCC10FC,0x3043F330,0x0FCCC0FC}  /* 6 pixels */
#define V210_HD_RED75     {0x350331B4,0x0CC6D0CC,0x1B433350,0x0CCD40CC}  /* 6 pixels */
#define V210_HD_BLUE75    {0x1E01C350,0x070D4070,0x3501C1E0,0x07078070}  /* 6 pixels */
#define V210_HD_BLACK75   {0x20010200,0x04080040,0x20010200,0x04080040}  /* 6 pixels */

#define Y444_WHITE100_Q     0x0080EB80  //0x80EA80 0x VV YY UU
#define Y444_YELLOW100_Q    0x008ADB10  //0x8ADA10
#define Y444_CYAN100_Q      0x0010BC9A  //0x10BC9A
#define Y444_GREEN100_Q     0x001AAD2A  //0x1BAC2A
#define Y444_MAGENTA100_Q   0x00E64ED6  //0xE54ED6
#define Y444_RED100_Q       0x00F03F66  //0xF03E66
#define Y444_BLUE100_Q      0x007620F0  //0x7620F0
#define Y444_BLACK100_Q     0x00801080  //0x801080

#define YUV42210BE_WHITE_100   {0x80,0x3A,0xB8,0x03,0xAC} /* 2 pixels - 5 bytes */
#define YUV42210BE_YELLOW_100  {0x10,0x36,0xB8,0x2B,0xAC} /* 2 pixels - 5 bytes */
#define YUV42210BE_CYAN_100    {0x99,0x6F,0xC1,0x42,0xF0} /* 2 pixels - 5 bytes */
#define YUV42210BE_GREEN_100   {0x29,0x6B,0xC1,0x6A,0xB0} /* 2 pixels - 5 bytes */
#define YUV42210BE_MAGENTA_100 {0xD6,0x93,0xEE,0x95,0x38} /* 2 pixels - 5 bytes */
#define YUV42210BE_RED_100     {0x66,0x8F,0xEE,0xBC,0xF8} /* 2 pixels - 5 bytes */
#define YUV42210BE_BLACK_100   {0x80,0x04,0x08,0x00,0x40} /* 2 pixels - 5 bytes */
#define YUV42210BE_BLUE_100    {0xEF,0xC7,0xF7,0xD4,0x7C} /* 2 pixels - 5 bytes */

#define R444_WHITE100_Q     0x00FFFFFF  //0xFFFFFF
#define R444_YELLOW100_Q    0x0000FFFF  //0x00FFFF
#define R444_CYAN100_Q      0x00FFFF00  //0xFFFF00
#define R444_GREEN100_Q     0x0000FF00  //0x00FF00
#define R444_MAGENTA100_Q   0x00FF00FF  //0xFF00FF
#define R444_RED100_Q       0x000000FF  //0x0000FF
#define R444_BLUE100_Q      0x00FF0000  //0xFF0000
#define R444_BLACK100_Q     0x00000000  //0x000000

#define BGR444_WHITE100_Q    0x00FFFFFF  // BGR: 0xFFFFFF
#define BGR444_YELLOW100_Q   0x00FFFF00  // BGR: 0xFFFF00
#define BGR444_CYAN100_Q     0x0000FFFF  // BGR: 0x00FFFF
#define BGR444_GREEN100_Q    0x0000FF00  // BGR: 0x00FF00
#define BGR444_MAGENTA100_Q  0x00FF00FF  // BGR: 0xFF00FF
#define BGR444_RED100_Q      0x00FF0000  // BGR: 0xFF0000
#define BGR444_BLUE100_Q     0x000000FF  // BGR: 0x0000FF
#define BGR444_BLACK100_Q    0x00000000  // BGR: 0x000000

#define NV12_WHITE100_Y      0xFF
#define NV12_WHITE100_UV     0x8080
#define NV12_YELLOW100_Y     0xDB
#define NV12_YELLOW100_UV    0x1092
#define NV12_CYAN100_Y       0xBB
#define NV12_CYAN100_UV      0xA610
#define NV12_GREEN100_Y      0xAC
#define NV12_GREEN100_UV     0x2236
#define NV12_MAGENTA100_Y    0x6E
#define NV12_MAGENTA100_UV   0xCADE
#define NV12_RED100_Y        0x52
#define NV12_RED100_UV       0x5AFF
#define NV12_BLUE100_Y       0x2E
#define NV12_BLUE100_UV      0xF010
#define NV12_BLACK100_Y      0x00
#define NV12_BLACK100_UV     0x8080

#define P010_WHITE100_Y      0xEB00
#define P010_WHITE100_UV     0x80008000
#define P010_YELLOW100_Y     0xDB40
#define P010_YELLOW100_UV    0x10008A40
#define P010_CYAN100_Y       0xBC80
#define P010_CYAN100_UV      0x99C01000
#define P010_GREEN100_Y      0xACC0
#define P010_GREEN100_UV     0x29C01A40
#define P010_MAGENTA100_Y    0x4E40
#define P010_MAGENTA100_UV   0xD640E5C0
#define P010_RED100_Y        0x3E80
#define P010_RED100_UV       0x6640F000
#define P010_BLUE100_Y       0x1FC0
#define P010_BLUE100_UV      0xF00075C0
#define P010_BLACK100_Y      0x1000
#define P010_BLACK100_UV     0x80008000


ColorBar::ColorBar(int width, int height, PixelFormat pixel_format):
   m_pixel_format(pixel_format)
   ,m_width(width)
   ,m_height(height)
{
   switch (pixel_format)
   {
   case PixelFormat::ycbcr_422_8:
      init_ycbcr422_8(width, height);
      break;
   case PixelFormat::ycbcr_422_10_le_msb:
      init_ycbcr_422_10_le_msb(width, height);
      break;
   case PixelFormat::ycbcr_444_8:
      init_ycbcr_444_8(width, height);
      break;
   case PixelFormat::ycbcr_422_10_be:
      init_ycbcr_422_10_be(width, height);
      break;
   case PixelFormat::rgb_444_8:
      init_rgb_444_8(width, height);
      break;
   case PixelFormat::bgr_444_8_le_msb:
      init_bgr_444_8_le_msb(width, height);
      break;
   case PixelFormat::bgr_444_8:
      init_bgr_444_8(width, height);
      break;
   case PixelFormat::nv12:
      init_nv12(width, height);
      break;
   case PixelFormat::p010:
      init_p010(width, height);
      break;
   default: break;
   }
}

ColorBar::~ColorBar()
{
   if(m_pattern)
      delete[] m_pattern;
}

uint8_t* ColorBar::get_data()
{
   return m_pattern;
}

uint64_t ColorBar::get_datasize()
{
   return m_datasize;
}

void ColorBar::init_ycbcr_422_10_le_msb(int width, int height)
{
   DataV210 color[] = { V210_HD_WHITE75,V210_HD_YELLOW75,V210_HD_CYAN75,V210_HD_GREEN75,V210_HD_MAGENTA75,V210_HD_RED75,V210_HD_BLUE75,V210_HD_BLACK75 };
   m_datasize = width * height * 8 / 3;
   m_pattern = new uint8_t[m_datasize];
   if (m_pattern)
   {
      DataV210* pData_X = (DataV210*)m_pattern;

      for (int pixel = 0; pixel < width * height; pixel += 6)
      {
         *pData_X = color[(pixel % width) / (width / 8)];
         pData_X++;
      }
   }
}

void ColorBar::init_ycbcr_422_10_be(int width, int height)
{
   DataYUV42210BE color[] = { YUV42210BE_WHITE_100,  YUV42210BE_YELLOW_100 , YUV42210BE_CYAN_100 , YUV42210BE_GREEN_100 , YUV42210BE_MAGENTA_100, YUV42210BE_RED_100 , YUV42210BE_BLUE_100, YUV42210BE_BLACK_100 };
   m_datasize = width * height * 5 / 2;
   m_pattern = new uint8_t[m_datasize];
   if (m_pattern)
   {
      DataYUV42210BE* pData_X = (DataYUV42210BE*)m_pattern;
      for (int pixel = 0; pixel < width * height; pixel += 2)
      {
         *pData_X = color[(pixel % width) / (width / 8)];
         pData_X++;
      }
   }
}

void ColorBar::init_ycbcr422_8(int width, int height)
{
   uint32_t color[]= { V208_WHITE75, V208_YELLOW75, V208_CYAN75, V208_GREEN75, V208_MAGENTA75, V208_RED75, V208_BLUE75, V208_BLACK75 };
   m_datasize = (uint64_t)width * height * 2;
   m_pattern = new uint8_t[m_datasize];

   for (uint64_t x = 0; x < width; x += 2)
   {
      for (uint64_t y = 0; y < height; y++)
      {
         *(uint32_t*)(m_pattern + (x * 2) + (y * width * 2)) = color[(x * 8) / width];
      }
   }
}

void ColorBar::init_ycbcr_444_8(int width, int height)
{
   uint32_t color[] = { Y444_WHITE100_Q, Y444_YELLOW100_Q, Y444_CYAN100_Q, Y444_GREEN100_Q, Y444_MAGENTA100_Q, Y444_RED100_Q, Y444_BLUE100_Q, Y444_BLACK100_Q };
   m_datasize = (uint64_t)width * height * 3;
   m_pattern = new uint8_t[m_datasize];

   for (uint64_t x = 0; x < width; x++)
   {
      for (uint64_t y = 0; y < height; y++)
      {
         uint32_t color_index = (x * 8) / width;
         uint32_t color_value = color[color_index];
         m_pattern[(x * 3) + (y * width * 3)] = (color_value >> 16) & 0xFF;
         m_pattern[(x * 3 + 1) + (y * width * 3)] = (color_value >> 8) & 0xFF;
         m_pattern[(x * 3 + 2) + (y * width * 3)] = color_value & 0xFF;
      }
   }
}

void ColorBar::init_rgb_444_8(int width, int height)
{
   uint32_t color[] = { R444_WHITE100_Q, R444_YELLOW100_Q, R444_CYAN100_Q, R444_GREEN100_Q, R444_MAGENTA100_Q, R444_RED100_Q, R444_BLUE100_Q, R444_BLACK100_Q };
   m_datasize = (uint64_t)width * height * 3;
   m_pattern = new uint8_t[m_datasize];

   for (uint64_t x = 0; x < width; x++)
   {
      for (uint64_t y = 0; y < height; y++)
      {
         uint32_t color_index = (x * 8) / width;
         uint32_t color_value = color[color_index];
         m_pattern[(x * 3) + (y * width * 3)] = (color_value >> 16) & 0xFF;
         m_pattern[(x * 3 + 1) + (y * width * 3)] = (color_value >> 8) & 0xFF;
         m_pattern[(x * 3 + 2) + (y * width * 3)] = color_value & 0xFF;
      }
   }
}

void ColorBar::init_bgr_444_8(int width, int height)
{
   uint32_t color[] = { BGR444_WHITE100_Q, BGR444_YELLOW100_Q, BGR444_CYAN100_Q, BGR444_GREEN100_Q, BGR444_MAGENTA100_Q, BGR444_RED100_Q, BGR444_BLUE100_Q, BGR444_BLACK100_Q };
   m_datasize = (uint64_t)width * height * 3;
   m_pattern = new uint8_t[m_datasize];

   for (uint64_t x = 0; x < width; x++)
   {
      for (uint64_t y = 0; y < height; y++)
      {
         uint32_t color_index = (x * 8) / width;
         uint32_t color_value = color[color_index];
         m_pattern[(x * 3) + (y * width * 3)] = (color_value >> 16) & 0xFF;
         m_pattern[(x * 3 + 1) + (y * width * 3)] = (color_value >> 8) & 0xFF;
         m_pattern[(x * 3 + 2) + (y * width * 3)] = color_value & 0xFF;
      }
   }

}

void ColorBar::init_bgr_444_8_le_msb(int width, int height)
{
   uint32_t color[] = { BGR444_WHITE100_Q, BGR444_YELLOW100_Q, BGR444_CYAN100_Q, BGR444_GREEN100_Q, BGR444_MAGENTA100_Q, BGR444_RED100_Q, BGR444_BLUE100_Q, BGR444_BLACK100_Q };
   m_datasize = (uint32_t)width * height * 4;
   m_pattern = new uint8_t[m_datasize];
   auto m_pattern_32 = (uint32_t*)m_pattern;

   for (uint32_t x = 0; x < width; x++)
   {
      for (uint32_t y = 0; y < height; y++)
      {
         *(m_pattern_32 + x + y * width) = color[(x * 8) / width];
      }
   }
}

void ColorBar::init_nv12(int width, int height)
{
   uint8_t y_colors[] = { NV12_WHITE100_Y, NV12_YELLOW100_Y, NV12_CYAN100_Y, NV12_GREEN100_Y,
                          NV12_MAGENTA100_Y, NV12_RED100_Y, NV12_BLUE100_Y, NV12_BLACK100_Y };
   uint16_t uv_colors[] = { NV12_WHITE100_UV, NV12_YELLOW100_UV, NV12_CYAN100_UV, NV12_GREEN100_UV,
                            NV12_MAGENTA100_UV, NV12_RED100_UV, NV12_BLUE100_UV, NV12_BLACK100_UV };

   m_datasize = (uint64_t)width * height * 3 / 2;
   m_pattern = new uint8_t[m_datasize];

   if (m_pattern)
   {
      uint8_t* y_plane = m_pattern;
      for (uint64_t y = 0; y < height; y++)
      {
         for (uint64_t x = 0; x < width; x++)
         {
            uint32_t color_index = (x * 8) / width;
            y_plane[y * width + x] = y_colors[color_index];
         }
      }

      uint8_t* uv_plane = m_pattern + (width * height);
      for (uint64_t y = 0; y < height / 2; y++)
      {
         for (uint64_t x = 0; x < width / 2; x++)
         {
            uint32_t color_index = ((x * 2) * 8) / width;
            uint16_t uv_value = uv_colors[color_index];
            uv_plane[y * width + x * 2] = (uv_value >> 8) & 0xFF;  // U
            uv_plane[y * width + x * 2 + 1] = uv_value & 0xFF;     // V
         }
      }
   }
}

void ColorBar::init_p010(int width, int height)
{
   uint16_t y_colors[] = { P010_WHITE100_Y, P010_YELLOW100_Y, P010_CYAN100_Y, P010_GREEN100_Y,
                          P010_MAGENTA100_Y, P010_RED100_Y, P010_BLUE100_Y, P010_BLACK100_Y };
   uint32_t uv_colors[] = { P010_WHITE100_UV, P010_YELLOW100_UV, P010_CYAN100_UV, P010_GREEN100_UV,
                            P010_MAGENTA100_UV, P010_RED100_UV, P010_BLUE100_UV, P010_BLACK100_UV };

   m_datasize = (uint64_t)width * height * 3;
   m_pattern = new uint8_t[m_datasize];

   if (m_pattern)
   {
      uint16_t* y_plane = reinterpret_cast<uint16_t*>(m_pattern);
      for (uint64_t y = 0; y < height; y++)
      {
         for (uint64_t x = 0; x < width; x++)
         {
            uint32_t color_index = (x * 8) / width;
            y_plane[y * width + x] = y_colors[color_index];
         }
      }

      uint16_t* uv_plane = reinterpret_cast<uint16_t*>(m_pattern + (width * height) * 2);
      for (uint64_t y = 0; y < height / 2; y++)
      {
         for (uint64_t x = 0; x < width / 2; x++)
         {
            uint32_t color_index = ((x * 2) * 8) / width;
            uint32_t uv_value = uv_colors[color_index];
            uv_plane[y * width + x * 2] = (uv_value >> 16) & 0xFFFF;
            uv_plane[y * width + x * 2 + 1] = uv_value & 0xFFFF;
         }
      }
   }
}

void ColorBar::draw_moving_line(uint8_t* data, int frame_count)
{
   switch (m_pixel_format)
   {
   case PixelFormat::ycbcr_422_8:
      draw_moving_line_ycbcr_422_8(data, frame_count);
      break;
   case PixelFormat::ycbcr_422_10_le_msb:
      draw_moving_line_ycbcr_422_10_le_msb(data, frame_count);
      break;
   case PixelFormat::ycbcr_444_8:
      draw_moving_line_ycbcr_444_8(data, frame_count);
      break;
   case PixelFormat::ycbcr_422_10_be:
      draw_moving_line_ycbcr_422_10_be(data, frame_count);
      break;
   case PixelFormat::rgb_444_8:
      draw_moving_line_rgb_444_8(data, frame_count);
      break;
   case PixelFormat::bgr_444_8_le_msb:
      draw_moving_line_bgr_444_8_le_msb(data, frame_count);
      break;
   case PixelFormat::bgr_444_8:
      draw_moving_line_bgr_444_8(data, frame_count);
      break;
   case PixelFormat::nv12:
      draw_moving_line_nv12(data, frame_count);
      break;
   case PixelFormat::p010:
      draw_moving_line_p010(data, frame_count);
      break;
   default: break;
   }
}

void ColorBar::draw_moving_line_ycbcr_422_8(uint8_t* data, int frame_count)
{
   for(uint64_t x = 0; x < m_width; x += 2)
      *(uint32_t*)(data + (x * 2) + ((uint64_t)frame_count%m_height) * m_width * 2) = V208_WHITE75;
}

void ColorBar::draw_moving_line_ycbcr_422_10_le_msb(uint8_t* data, int frame_count)
{
   int line = frame_count % m_height;

   if (line * m_width * 8 % 3 == 0)
   {
      DataV210* pData_X = (DataV210*)(data + line * m_width * 8 / 3);
      for (int x = 0; x < m_width; x += 6)
      {
         *pData_X = V210_HD_WHITE75;
         pData_X++;
      }
   }
   else
   {
      int pxl = line * m_width;
      pxl += (6 - pxl % 6);

      DataV210* pData_X = (DataV210*)(data + pxl*8/3);

      for (int x = 0; x < m_width-6; x += 6)
      {
         *pData_X = V210_HD_WHITE75;
         pData_X++;
      }
   }


}

void ColorBar::draw_moving_line_ycbcr_422_10_be(uint8_t* data, int frame_count)
{
   DataYUV42210BE* pData_X = (DataYUV42210BE*)(data + frame_count % m_height * m_width * 5 / 2);
   for (int x = 0; x < m_width; x += 2)
   {
      *pData_X = YUV42210BE_WHITE_100;
      pData_X++;
   }

}

void ColorBar::draw_moving_line_ycbcr_444_8(uint8_t* data, int frame_count)
{
   uint8_t* line = data + ((uint64_t)frame_count % m_height) * m_width * 3;
   for (uint64_t x = 0; x < m_width; x++)
   {
      line[x * 3 + 0] = (Y444_WHITE100_Q >> 16) & 0xFF;
      line[x * 3 + 1] = (Y444_WHITE100_Q >> 8) & 0xFF;
      line[x * 3 + 2] = Y444_WHITE100_Q & 0xFF;
   }
}

void ColorBar::draw_moving_line_rgb_444_8(uint8_t* data, int frame_count)
{
   uint8_t* line = data + ((uint64_t)frame_count % m_height) * m_width * 3;
   for (uint64_t x = 0; x < m_width; x++)
   {
      line[x * 3 + 0] = (R444_WHITE100_Q >> 16) & 0xFF;
      line[x * 3 + 1] = (R444_WHITE100_Q >> 8) & 0xFF;
      line[x * 3 + 2] = R444_WHITE100_Q & 0xFF;
   }
}

void ColorBar::draw_moving_line_bgr_444_8(uint8_t* data, int frame_count)
{
   uint8_t* line = data + ((uint64_t)frame_count % m_height) * m_width * 3;
   for (uint64_t x = 0; x < m_width; x++)
   {
      line[x * 3 + 0] = (BGR444_WHITE100_Q >> 16) & 0xFF;
      line[x * 3 + 1] = (BGR444_WHITE100_Q >> 8) & 0xFF;
      line[x * 3 + 2] = BGR444_WHITE100_Q & 0xFF;
   }
}

void ColorBar::draw_moving_line_bgr_444_8_le_msb(uint8_t* data, int frame_count)
{
   uint32_t* line = reinterpret_cast<uint32_t*>(data + ((uint64_t)frame_count % m_height) * m_width * 4);
   for (uint64_t x = 0; x < m_width; x++)
      line[x] = BGR444_WHITE100_Q;
}

void ColorBar::draw_moving_line_nv12(uint8_t* data, int frame_count)
{
   int line_y = frame_count % m_height;

   uint8_t* y_plane = data;
   uint8_t* y_line = y_plane + line_y * m_width;
   for (uint64_t x = 0; x < m_width; x++)
   {
      y_line[x] = NV12_WHITE100_Y;
   }

   if (line_y % 2 == 0)
   {
      uint8_t* uv_plane = data + (m_width * m_height);
      uint8_t* uv_line = uv_plane + (line_y / 2) * m_width;
      for (uint64_t x = 0; x < m_width; x += 2)
      {
         uv_line[x] = (NV12_WHITE100_UV >> 8) & 0xFF;
         uv_line[x + 1] = NV12_WHITE100_UV & 0xFF;
      }
   }
}

void ColorBar::draw_moving_line_p010(uint8_t* data, int frame_count)
{
   int line_y = frame_count % m_height;

   uint16_t* y_plane = reinterpret_cast<uint16_t *>(data);
   uint16_t* y_line = y_plane + line_y * m_width;
   for (uint64_t x = 0; x < m_width; x++)
   {
      y_line[x] = P010_WHITE100_Y;
   }

   if (line_y % 2 == 0)
   {
      uint16_t* uv_plane = reinterpret_cast<uint16_t *>(data + (m_width * m_height * 2));
      uint16_t* uv_line = uv_plane + (line_y / 2) * m_width;
      for (uint64_t x = 0; x < m_width; x += 2)
      {
         uv_line[x] = (P010_WHITE100_UV >> 16) & 0xFFFF;
         uv_line[x + 1] = P010_WHITE100_UV & 0xFFFF;
      }
   }
}
