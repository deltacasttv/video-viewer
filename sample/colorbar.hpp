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

#pragma once

#include <stdint.h>

namespace Deltacast
{
   class ColorBar 
   {
   public:

      enum class PixelFormat {
         ycbcr_422_8, /*< YCbCr 4:2:2 8bit without any padding */
         ycbcr_422_10_le_msb, /*< YCbCr 4:2:2 10bit little endian with msb padding (30/32 bits) */
         ycbcr_422_10_be, /*< YCbCr 4:2:2 10bit big endian without any padding */
         reserved_ycbcr_422_12_le_msb, /*< YCbCr 4:2:2 12bit little endian with msb padding (12/16 bits) */
         reserved_ycbcr_422_12_be, /*< YCbCr 4:2:2 12bit big endian without any padding */
         reserved_ycbcr_422_16_le, /*< YCbCr 4:2:2 16bit little endian without any padding */
         reserved_ycbcr_422_16_be, /*< YCbCr 4:2:2 16bit big endian without any padding */
         ycbcr_444_8, /*< YCbCr 4:4:4 8bit without any padding */
         reserved_ycbcr_444_10_le_msb, /*< YCbCr 4:2:2 10bit little endian with msb padding (30/32 bits) */
         reserved_ycbcr_444_10_be, /*< YCbCr 4:4:4 10bit big endian without any padding */
         reserved_ycbcr_444_12_le_msb, /*< YCbCr 4:4:4 12bit little endian with msb padding (12/16 bits) */
         reserved_ycbcr_444_12_be, /*< YCbCr 4:4:4 12bit big endian without any padding */
         reserved_ycbcr_444_16_le, /*< YCbCr 4:4:4 16bit little endian without any padding */
         reserved_ycbcr_444_16_be, /*< YCbCr 4:4:4 16bit big endian without any padding */
         rgb_444_8, /*< RGB 4:4:4 8bit without any padding */
         reserved_rgb_444_10_le_msb, /*< RGB 4:2:2 10bit little endian with msb padding (30/32 bits) */
         reserved_rgb_444_10_be, /*< RGB 4:4:4 10bit big endian without any padding */
         reserved_rgb_444_12_le_msb, /*< RGB 4:4:4 12bit little endian with msb padding (12/16 bits) */
         reserved_rgb_444_12_be, /*< RGB 4:4:4 12bit big endian without any padding */
         reserved_rgb_444_16_le, /*< RGB 4:4:4 16bit little endian without any padding */
         reserved_rgb_444_16_be, /*< RGB 4:4:4 16bit big endian without any padding */
         bgr_444_8_le_msb, /*< BGR 4:4:4 8bit little endian with msb padding (24/32 bits) */
         bgr_444_8, /*< BGR 4:4:4 8bit without any padding */
         nv12,
         p010,
         nb_pixel_format
      };

      ColorBar(int width, int height, PixelFormat pixel_format);
      ~ColorBar();

      uint8_t* get_data();
      uint64_t get_datasize();
      void draw_moving_line(uint8_t* data, int frame_count);

   private:
      void init_ycbcr422_8(int width, int height);
      void init_ycbcr_422_10_le_msb(int width, int height);
      void init_ycbcr_422_10_be(int width, int height);
      void init_ycbcr_444_8(int width, int height);
      void init_rgb_444_8(int width, int height);
      void init_bgr_444_8(int width, int height);
      void init_bgr_444_8_le_msb(int width, int height);
      void init_nv12(int width, int height);
      void init_p010(int width, int height);
      void draw_moving_line_ycbcr_422_8(uint8_t* data, int frame_count);
      void draw_moving_line_ycbcr_422_10_le_msb(uint8_t* data, int frame_count);
      void draw_moving_line_ycbcr_444_8(uint8_t* data, int frame_count);
      void draw_moving_line_rgb_444_8(uint8_t* data, int frame_count);
      void draw_moving_line_bgr_444_8(uint8_t* data, int frame_count);
      void draw_moving_line_ycbcr_422_10_be(uint8_t* data, int frame_count);
      void draw_moving_line_bgr_444_8_le_msb(uint8_t* data, int frame_count);
      void draw_moving_line_nv12(uint8_t* data, int frame_count);
      void draw_moving_line_p010(uint8_t* data, int frame_count);

      uint8_t* m_pattern{nullptr};
      uint64_t m_datasize{0};
      PixelFormat m_pixel_format{PixelFormat::rgb_444_8};
      int m_width{0};
      int m_height{0};
   };
} 