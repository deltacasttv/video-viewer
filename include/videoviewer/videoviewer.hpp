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

#ifdef __GNUC__
#include <stdint-gcc.h>
#else
#include <stdint.h>
#endif

namespace Deltacast
{
   class VideoViewer
   {
      public:

      enum class InputFormat {
         ycbcr_422_8, /*< YCbCr 4:2:2 8bit without any padding */
         ycbcr_422_10_le_msb, /*< YCbCr 4:2:2 10bit little endian with msb padding (30/32 bits) */
         ycbcr_422_10_be, /*< YCbCr 4:2:2 10bit big endian without any padding */
         reserved_ycbcr_422_12_le_msb, /*< YCbCr 4:2:2 12bit little endian with msb padding (12/16 bits) */
         reserved_ycbcr_422_12_be, /*< YCbCr 4:2:2 12bit big endian without any padding */
         reserved_ycbcr_422_16_le, /*< YCbCr 4:2:2 16bit little endian without any padding */
         reserved_ycbcr_422_16_be, /*< YCbCr 4:2:2 12bit big endian without any padding */
         ycbcr_444_8, /*< YCbCr 4:4:4 8bit without any padding */
         reserved_ycbcr_444_10_le_msb, /*< YCbCr 4:2:2 10bit little endian with msb padding (30/32 bits) */
         reserved_ycbcr_444_10_be, /*< YCbCr 4:4:4 10bit big endian without any padding */
         reserved_ycbcr_444_12_le_msb, /*< YCbCr 4:4:4 12bit little endian with msb padding (12/16 bits) */
         reserved_ycbcr_444_12_be, /*< YCbCr 4:4:4 12bit big endian without any padding */
         reserved_ycbcr_444_16_le, /*< YCbCr 4:4:4 16bit little endian without any padding */
         reserved_ycbcr_444_16_be, /*< YCbCr 4:4:4 12bit big endian without any padding */
         rgb_444_8, /*< RGB 4:4:4 8bit without any padding */
         reserved_rgb_444_10_le_msb, /*< RGB 4:2:2 10bit little endian with msb padding (30/32 bits) */
         reserved_rgb_444_10_be, /*< RGB 4:4:4 10bit big endian without any padding */
         reserved_rgb_444_12_le_msb, /*< RGB 4:4:4 12bit little endian with msb padding (12/16 bits) */
         reserved_rgb_444_12_be, /*< RGB 4:4:4 12bit big endian without any padding */
         reserved_rgb_444_16_le, /*< RGB 4:4:4 16bit little endian without any padding */
         reserved_rgb_444_16_be, /*< RGB 4:4:4 12bit big endian without any padding */
         nb_input_format
      };

      VideoViewer();
      ~VideoViewer();

      /*!
      * @brief Initialization function that creates opengl window and textures
      *
      * Limitation: this function should be called in the same thread that calls VV_render_loop to allow window events to be processed
      *
      * @returns the status of its execution (true = no error or false = error)
      */
      bool init(int window_width, int window_height, const char* window_title, int texture_width, int texture_height, InputFormat input_format);

      /*!
      * @brief Initialization function that creates opengl window and textures at a given position
      *
      * Limitation: this function should be called in the same thread that calls VV_render_loop to allow window events to be processed
      *
      * @returns the status of its execution (true = no error or false = error)
      */
      bool init(int window_width, int window_height, const char* window_title, int texture_width, int texture_height, InputFormat input_format, int xpos, int ypos);


      /*!
      * @brief Release function that destroy opengl window and textures
      *
      * Ensure that VV_render_loop returns before calling VV_release function.
      *
      * @returns the status of its execution (true = no error or false = error)
      */
      bool release();

      /*!
      * @brief Loop function that renders textures in the opengl context
      *
      * Limitation: this function should be called in the same thread that calls VV_init to allow window events to be processed
      *
      * @returns the status of its execution (true = no error or false = error)
      */
      bool render_loop(int frame_rate_in_ms);

      /*!
      * @brief Stop function that forces VV_reder_loop function to return
      */
      void stop();

      /*!
      * @brief Function that provides pointer to data memory space.
      *
      * If success, this function must be followed by call to VV_unlock_data
      *
      * @returns the status of its execution (true = no error or false = error)
      */
      bool lock_data(uint8_t** data, uint64_t* size);

      /*!
      * @brief Function unlock previously locked data.
      *
      * Data pointer cannot be accessed anymore after this call.
      */
      void unlock_data();

      /*!
      * @brief Function that allows user to know if the opengl window is closing (or closed)
      *
      * Opengl window can be closed by user interface event
      *
      * @returns the status of its execution (true = no error or false = error)
      */
      bool window_request_close();


      /*!
      * @brief Function that allows user to set the windows title
      *
      * @returns the status of its execution (true = no error or false = error)
      */
      bool window_set_title(const char* title);

   };
}