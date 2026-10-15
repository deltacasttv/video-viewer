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

#include <functional>
#include <stdint.h>

namespace Deltacast
{
   class VideoViewer
   {
      public:

      /*!
      * @brief Layout of the frames written through lock_data()
      *
      * Rules shared by all supported formats (W = texture_width and H = texture_height, as given to init()):
      * - The buffer holds one progressive frame of W x H pixels. Lines are stored from top to bottom and
      *   pixels from left to right.
      * - Lines are tightly packed: there is no line padding and no configurable stride (see
      *   ycbcr_422_10_le_msb for the way its lines follow each other).
      * - The size returned by lock_data() is exactly the buffer size given below for the format.
      * - Yn is the luma sample of pixel n of a line, Cbn and Crn are the chroma samples stored with
      *   pixel n, and S[i:j] means bits i down to j of sample S (bit 0 is the least significant bit).
      * - 4:2:2 formats: the Cb/Cr pair stored with pixel 2n is used for pixels 2n and 2n+1.
      *   4:2:0 formats: the Cb/Cr pair at column n and line m of the chroma plane is used for the
      *   2 x 2 pixels at columns 2n, 2n+1 and lines 2m, 2m+1.
      * - YCbCr formats are converted to RGB with BT.709 coefficients and expect limited ("video") range
      *   values: Y in [16, 235] and Cb/Cr in [16, 240] for 8-bit samples, Y in [64, 940] and Cb/Cr in
      *   [64, 960] for 10-bit samples. RGB formats are displayed as they are (full range).
      * - init() returns false when W or H does not meet the constraints of the format. Constraints marked
      *   "not checked" are not verified by init(): if they are not met, the frame is displayed garbled.
      * - Formats whose name starts with "reserved_" are not supported: init() returns false.
      */
      enum class InputFormat {
         /*!
         * @brief YCbCr 4:2:2 8-bit, "UYVY" byte order
         *
         * - 4 bytes per pair of pixels: Cb0, Y0, Cr0, Y1, then Cb2, Y2, Cr2, Y3, and so on.
         * - Line size: W * 2 bytes. Buffer size: W * H * 2 bytes.
         * - Constraint: W even.
         */
         ycbcr_422_8, /*< YCbCr 4:2:2 8bit without any padding */
         /*!
         * @brief YCbCr 4:2:2 10-bit, "v210" packing: 3 samples per 32-bit little-endian word
         *
         * - Each 32-bit little-endian word holds three 10-bit samples in bits [9:0], [19:10] and [29:20].
         *   Bits [31:30] are padding and are ignored.
         * - 6 pixels are packed in 4 words (16 bytes):
         *     word 0 = Cb0 | Y0 << 10 | Cr0 << 20
         *     word 1 = Y1  | Cb2 << 10 | Y2 << 20
         *     word 2 = Cr2 | Y3 << 10 | Cb4 << 20
         *     word 3 = Y4  | Cr4 << 10 | Y5 << 20
         * - The groups of 6 pixels form one continuous stream over the whole frame: pixel x of line y is
         *   pixel x + y * W of the stream. Lines are not padded, so when W is not a multiple of 6, a group
         *   spans the end of a line and the start of the next one. v210 buffers with padded lines (commonly
         *   aligned on 128 bytes) are only displayed correctly when the padding is empty, which for
         *   128-byte alignment means W multiple of 48 (e.g. 720, 1920 or 3840).
         * - Buffer size: W * H * 8 / 3 bytes.
         * - Constraints: H multiple of 3 and W * H * 8 multiple of 3; W even (not checked).
         */
         ycbcr_422_10_le_msb, /*< YCbCr 4:2:2 10bit little endian with msb padding (30/32 bits) */
         /*!
         * @brief YCbCr 4:2:2 10-bit big endian without padding: 5 bytes per pair of pixels
         *
         * - The samples Cb0, Y0, Cr0, Y1, Cb2, Y2, Cr2, Y3, ... are packed as a continuous stream of
         *   10-bit values, most significant bit first. Each pair of pixels is therefore the 40-bit
         *   big-endian value Cb0 << 30 | Y0 << 20 | Cr0 << 10 | Y1, i.e. (from bit 7 to bit 0):
         *     byte 0 = Cb0[9:2]
         *     byte 1 = Cb0[1:0] Y0[9:4]
         *     byte 2 = Y0[3:0] Cr0[9:6]
         *     byte 3 = Cr0[5:0] Y1[9:8]
         *     byte 4 = Y1[7:0]
         * - Line size: W * 5 / 2 bytes. Buffer size: W * H * 5 / 2 bytes.
         * - Constraints: W * H even; W multiple of 8 (not checked).
         * - Known limitation of the current implementation: the conversion shader only finds the right
         *   position in each 8-pixel group on every line when W is a multiple of 64 (e.g. 1280, 1920,
         *   2048, 3840 or 4096). With other widths, such as 720 or 1440, some lines are displayed garbled.
         */
         ycbcr_422_10_be, /*< YCbCr 4:2:2 10bit big endian without any padding */
         reserved_ycbcr_422_12_le_msb, /*< YCbCr 4:2:2 12bit little endian with msb padding (12/16 bits) */
         reserved_ycbcr_422_12_be, /*< YCbCr 4:2:2 12bit big endian without any padding */
         reserved_ycbcr_422_16_le, /*< YCbCr 4:2:2 16bit little endian without any padding */
         reserved_ycbcr_422_16_be, /*< YCbCr 4:2:2 16bit big endian without any padding */
         /*!
         * @brief YCbCr 4:4:4 8-bit
         *
         * - 3 bytes per pixel: Cb, Y, Cr.
         * - Line size: W * 3 bytes. Buffer size: W * H * 3 bytes.
         * - Constraint: W multiple of 4 (W * 3 multiple of 4).
         */
         ycbcr_444_8, /*< YCbCr 4:4:4 8bit without any padding */
         reserved_ycbcr_444_10_le_msb, /*< YCbCr 4:4:4 10bit little endian with msb padding (30/32 bits) */
         reserved_ycbcr_444_10_be, /*< YCbCr 4:4:4 10bit big endian without any padding */
         reserved_ycbcr_444_12_le_msb, /*< YCbCr 4:4:4 12bit little endian with msb padding (12/16 bits) */
         reserved_ycbcr_444_12_be, /*< YCbCr 4:4:4 12bit big endian without any padding */
         reserved_ycbcr_444_16_le, /*< YCbCr 4:4:4 16bit little endian without any padding */
         reserved_ycbcr_444_16_be, /*< YCbCr 4:4:4 16bit big endian without any padding */
         /*!
         * @brief RGB 4:4:4 8-bit
         *
         * - 3 bytes per pixel: R, G, B.
         * - Line size: W * 3 bytes. Buffer size: W * H * 3 bytes.
         * - Constraint: W multiple of 4 (W * 3 multiple of 4).
         */
         rgb_444_8, /*< RGB 4:4:4 8bit without any padding */
         reserved_rgb_444_10_le_msb, /*< RGB 4:4:4 10bit little endian with msb padding (30/32 bits) */
         reserved_rgb_444_10_be, /*< RGB 4:4:4 10bit big endian without any padding */
         reserved_rgb_444_12_le_msb, /*< RGB 4:4:4 12bit little endian with msb padding (12/16 bits) */
         reserved_rgb_444_12_be, /*< RGB 4:4:4 12bit big endian without any padding */
         reserved_rgb_444_16_le, /*< RGB 4:4:4 16bit little endian without any padding */
         reserved_rgb_444_16_be, /*< RGB 4:4:4 16bit big endian without any padding */
         /*!
         * @brief RGB 4:4:4 8-bit, 3 bytes per pixel (currently the same byte order as rgb_444_8)
         *
         * - Current implementation: 3 bytes per pixel in the order R, G, B, exactly like rgb_444_8 (the
         *   buffer is uploaded as GL_BGR and the conversion shader swaps red and blue back). A buffer in
         *   B, G, R order is displayed with red and blue swapped.
         * - Line size: W * 3 bytes. Buffer size: W * H * 3 bytes.
         * - Constraint: W multiple of 4 (W * 3 multiple of 4).
         */
         bgr_444_8, /*< BGR 4:4:4 8bit without any padding */
         reserved_bgr_444_10_le_msb, /*< BGR 4:4:4 10bit little endian with msb padding (30/32 bits) */
         reserved_bgr_444_10_be, /*< BGR 4:4:4 10bit big endian without any padding */
         reserved_bgr_444_12_le_msb, /*< BGR 4:4:4 12bit little endian with msb padding (12/16 bits) */
         reserved_bgr_444_12_be, /*< BGR 4:4:4 12bit big endian without any padding */
         reserved_bgr_444_16_le, /*< BGR 4:4:4 16bit little endian without any padding */
         reserved_bgr_444_16_be, /*< BGR 4:4:4 16bit big endian without any padding */
         /*!
         * @brief BGR 4:4:4 8-bit with msb padding: one 32-bit little-endian word per pixel
         *
         * - 4 bytes per pixel: B, G, R, X, i.e. the little-endian word 0xXXRRGGBB. X (the most significant
         *   byte) is padding and is ignored.
         * - Line size: W * 4 bytes. Buffer size: W * H * 4 bytes.
         * - No constraint on W and H.
         */
         bgr_444_8_le_msb, /*< BGR 4:4:4 8bit with msb padding (32 bits) */
         /*!
         * @brief NV12: YCbCr 4:2:0 8-bit, semi-planar
         *
         * - Y plane at offset 0: H lines of W bytes, one Y sample per pixel.
         * - CbCr plane right after it, at offset W * H: H / 2 lines of W bytes, each holding W / 2
         *   interleaved pairs Cb, Cr.
         * - Buffer size: W * H * 3 / 2 bytes.
         * - Constraints: W and H even; W multiple of 4 (not checked).
         */
         nv12, /*< NV12 4:2:0 8bit without any padding */
         /*!
         * @brief P010: YCbCr 4:2:0 10-bit, semi-planar, one 16-bit little-endian word per sample
         *
         * - Each sample is a 16-bit little-endian word holding the 10-bit value in its most significant
         *   bits: word = sample << 6 (bits [5:0] should be 0).
         * - Y plane at offset 0: H lines of W words (W * 2 bytes), one Y sample per pixel.
         * - CbCr plane right after it, at offset W * H * 2: H / 2 lines of W words (W * 2 bytes), each
         *   holding W / 2 interleaved pairs Cb, Cr.
         * - Buffer size: W * H * 3 bytes.
         * - Constraints: W and H even.
         */
         p010, /*< P010 4:2:0 10bit padded on 16 bits */
         nb_input_format /*< Number of InputFormat values (reserved ones included), not a format */
      };

        VideoViewer();
        ~VideoViewer();

      /*!
      * @brief Initialization function that creates opengl window and textures
      *
      * Limitation: this function should be called in the same thread that calls render_loop or render_iteration to allow window events to be processed
      *
      * @param window_width Initial width of the window. The frame is stretched to fill the window.
      * @param window_height Initial height of the window
      * @param window_title Title of the window
      * @param texture_width Width of the frames, in pixels. It must meet the constraints of input_format (see InputFormat).
      * @param texture_height Height of the frames, in pixels. It must meet the constraints of input_format (see InputFormat).
      * @param input_format Layout of the frames written through lock_data() (see InputFormat)
      *
      * @returns the status of its execution (true = no error or false = error)
      */
      bool init(int window_width, int window_height, const char* window_title, int texture_width, int texture_height, InputFormat input_format);

      /*!
      * @brief Initialization function that creates opengl window and textures at a given position
      *
      * Limitation: this function should be called in the same thread that calls render_loop or render_iteration to allow window events to be processed
      *
      * @param xpos Horizontal position of the window
      * @param ypos Vertical position of the window
      *
      * The other parameters are the same as in the init() overload without position.
      *
      * @returns the status of its execution (true = no error or false = error)
      */
      bool init(int window_width, int window_height, const char* window_title, int texture_width, int texture_height, InputFormat input_format, int xpos, int ypos);


        /*!
         * @brief Release function that destroy opengl window and textures
         *
         * Ensure that render_loop or render_iteration  returns before calling the release()
         * function.
         *
         * @returns the status of its execution (true = no error or false = error)
         */
        bool release();

        /*!
         * @brief Loop function that renders textures in the opengl context. A sync function can be
         * provided to synchronize with other threads. By default, no synchronization is done.
         *
         * Limitation: this function should be called in the same thread that calls init to allow
         * window events to be processed
         *
         * @returns the status of its execution (true = no error or false = error)
         */
        bool render_loop(int frame_rate_in_ms, std::function<void()> sync_func = []() {});

        /*!
         * @brief Render iteration that renders textures in the opengl context
         *
         * Note: render_loop function should be preferred to this function but, in the case of the
         * rendering must be done in the main thread (i.e. on macOS), this function can be used to
         * make a custom loop
         *
         * Limitation: this function should be called in the same thread that calls init to allow
         * window events to be processed
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
      * The memory space holds one frame of texture_width x texture_height pixels in the input_format given
      * to init(), and size receives its size in bytes (see InputFormat for the layout and size of each format).
      * Its content is kept from one frame to the next and is uploaded at each render iteration.
      * This function fails when rendering is not active (render_loop not running, or start not called when
      * render_iteration is used) or when the window is closing.
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
         * Limitation: this function should be called in the same thread that calls render_iteration
         * to allow window events to be processed
         */
        void process_escape_key();
    };
}  // namespace Deltacast