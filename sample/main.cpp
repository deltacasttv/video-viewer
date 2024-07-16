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

#include <videoviewer/videoviewer.hpp>
#include "colorbar.hpp"
#include <thread>
#include <iostream>

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <string.h>
#include "../dep/keyboard.h"
#endif

bool stop = false;

#if defined(WIN32) || defined(_WIN32)
#define SLEEP(frame_rate_in_ms) Sleep(frame_rate_in_ms);
#define init_keyboard()
#define close_keyboard()
#else
#define SLEEP(frame_rate_in_ms) usleep(frame_rate_in_ms * 1000);
#endif

//This function is called inside a new thread that will be used to generate the pattern and copy it to the video buffer
void pattern_thread(Deltacast::VideoViewer& viewer,int frame_rate_in_ms, int width, int height, Deltacast::ColorBar::PixelFormat pixel_format)
{
   int frame_count = 0;
   uint8_t* data = nullptr;
   uint64_t size = 0;
   Deltacast::ColorBar color_bar(width, height, pixel_format);

   while (!stop)
   {
      if (viewer.lock_data(&data, &size)) {
         if (size == color_bar.get_datasize() && color_bar.get_data())
         {
            if (data)
            {
               memcpy(data, color_bar.get_data(), size);
               color_bar.draw_moving_line(data, frame_count);
            }
         }

         viewer.unlock_data();
      }
      else
         break;

      SLEEP(frame_rate_in_ms);
      frame_count++;
   }
}

//This function is called inside a new thread that will be used to render the video
void render_video(Deltacast::VideoViewer& viewer,int window_width, int window_height, const char* window_title, int texture_width, int texture_height, Deltacast::VideoViewer::InputFormat input_format, int frame_rate_in_ms)
{
   if (viewer.init(window_width, window_height, window_title, texture_width, texture_height, input_format))
   {
      viewer.render_loop(frame_rate_in_ms);
      viewer.release();
   }
   else
      std::cout << "VideoViewer initialization failed" << std::endl;
}

int main(int argc, char** argv) 
{
   Deltacast::VideoViewer viewer;
   init_keyboard();

   int texture_width = 1920;
   int texture_height = 1080;

   //Starting VideoViewer rendering inside a new thread   
   std::thread viewerthread(render_video, std::ref(viewer), 800, 600, "My window", texture_width, texture_height, Deltacast::VideoViewer::InputFormat::bgr_444_8_le_msb, 10);

   //Starting Pattern Generator inside a new thread
   std::thread pattern_generator(pattern_thread, std::ref(viewer), 10, texture_width, texture_height, Deltacast::ColorBar::PixelFormat::bgr_444_8_le_msb);

   std::cout << "Press any key to stop or close Opengl window..." << std::endl;
   while (!viewer.window_request_close())
   {
      if (_kbhit())
      {
         _getch();
         break;
      }
      SLEEP(100);
   }

   //Stopping Pattern Generator
   stop = true;
   pattern_generator.join();

   //Stopping VideoViewer
   viewer.stop();
   viewerthread.join();

   close_keyboard();

   return 0;
} 