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
#include <functional>

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
         bgr_444_8, /*< BGR 4:4:4 8bit without any padding */
         reserved_bgr_444_10_le_msb, /*< BGR 4:2:2 10bit little endian with msb padding (30/32 bits) */
         reserved_bgr_444_10_be, /*< BGR 4:4:4 10bit big endian without any padding */
         reserved_bgr_444_12_le_msb, /*< BGR 4:4:4 12bit little endian with msb padding (12/16 bits) */
         reserved_bgr_444_12_be, /*< BGR 4:4:4 12bit big endian without any padding */
         reserved_bgr_444_16_le, /*< BGR 4:4:4 16bit little endian without any padding */
         reserved_bgr_444_16_be, /*< BGR 4:4:4 12bit big endian without any padding */
         bgr_444_8_le_msb, /*< BGR 4:4:4 8bit with msb padding (32 bits) */
         nb_input_format
      };

      VideoViewer();
      ~VideoViewer();

      /*!
      * @brief Initialization function that creates opengl window and textures
      *
      * Limitation: this function should be called in the same thread that calls render_loop or render_iteration to allow window events to be processed
      *
      * @returns the status of its execution (true = no error or false = error)
      */
      bool init(int window_width, int window_height, const char* window_title, int texture_width, int texture_height, InputFormat input_format);

      /*!
      * @brief Initialization function that creates opengl window and textures at a given position
      *
      * Limitation: this function should be called in the same thread that calls render_loop or render_iteration to allow window events to be processed
      *
      * @returns the status of its execution (true = no error or false = error)
      */
      bool init(int window_width, int window_height, const char* window_title, int texture_width, int texture_height, InputFormat input_format, int xpos, int ypos);


      /*!
      * @brief Release function that destroy opengl window and textures
      *
      * Ensure that render_loop or render_iteration  returns before calling VV_release function.
      *
      * @returns the status of its execution (true = no error or false = error)
      */
      bool release();

      /*!
      * @brief Loop function that renders textures in the opengl context. A sync function can be provided to synchronize with other threads. By default, no synchronization is done.
      *
      * Limitation: this function should be called in the same thread that calls init to allow window events to be processed
      *
      * @returns the status of its execution (true = no error or false = error)
      */
      bool render_loop(int frame_rate_in_ms, std::function<void()> sync_func = [](){});

      /*!
      * @brief Render iteration that renders textures in the opengl context
      *
      * Note: render_loop function should be preferred to this function but, in the case of the rendering must be done in the main thread (i.e. on Macos), this function can be used to make a custom loop
      * 
      * Limitation: this function should be called in the same thread that calls init to allow window events to be processed
      */
      void render_iteration();

      /*!
      * @brief Start function that init internal variables of the library
      *
      * Note: must be only called in the context of the usage of render_iteration function
      */
      void start();

      /*!
      * @brief Stop function that forces render_loop function to return
      */
      void stop();

      /*!
      * @brief Function that provides pointer to data memory space.
      *
      * If successful, this function must be followed by a call to unlock_data
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
      * @brief Function that allows user to set the window title
      *
      * @returns the status of its execution (true = no error or false = error)
      */
      bool window_set_title(const char* title);

      /*!
      * @brief Function that allows user to close the window if the escape key is pressed
      * Note : must be called in the render_iteration usage case
      * Limitation: this function should be called in the same thread that calls render_iteration to allow window events to be processed
      */
      void process_escape_key();
   };
}