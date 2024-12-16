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
#include <chrono>
#include <atomic>
#include <condition_variable>

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <string.h>
#include "../dep/keyboard.h"
#endif

static const int desired_frame_rate = 60;
static const std::chrono::milliseconds frame_rate_in_ms(1000 / desired_frame_rate);


#if defined(WIN32) || defined(_WIN32)
#define init_keyboard()
#define close_keyboard()
#endif

//This function is called inside a new thread that will be used to generate the pattern and copy it to the video buffer
void pattern_thread(Deltacast::VideoViewer& viewer, int width, int height, Deltacast::ColorBar::PixelFormat pixel_format, std::atomic<bool>& stop, std::condition_variable& synchronisation_cv, std::mutex& synchronisation_mutex)
{
   int frame_count = 0;
   uint8_t* data = nullptr;
   uint64_t size = 0;
   Deltacast::ColorBar color_bar(width, height, pixel_format);

   while (!stop.load())
   {
      auto start_time = std::chrono::high_resolution_clock ::now();
      if (viewer.lock_data(&data, &size)) {
         if (size == color_bar.get_datasize() && color_bar.get_data())
         {
            if (data)
            {
               memcpy(data, color_bar.get_data(), size);
               color_bar.draw_moving_line(data, frame_count);
            }
         }
         else
            std::cout << "memcpy error: The color bar data size is different from the video buffer size or is null" << std::endl;

         viewer.unlock_data();
      }
      {
         std::unique_lock<std::mutex> lock(synchronisation_mutex);
         synchronisation_cv.notify_one();
      }
      auto elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
      std::this_thread::sleep_for(frame_rate_in_ms - elapsed_time);
      frame_count++;
   }
   synchronisation_cv.notify_one();
}

//Limitation: For Macos, the rendering is done in the main thread
#if !defined(__APPLE__)
//This function is called inside a new thread that will be used to render the video
void render_video(Deltacast::VideoViewer& viewer,int window_width, int window_height, const char* window_title, int texture_width, int texture_height, Deltacast::VideoViewer::InputFormat input_format, int frame_rate_in_ms, std::atomic<bool>& stop, std::condition_variable& synchronisation_cv, std::mutex& synchronisation_mutex)
{
   if (viewer.init(window_width, window_height, window_title, texture_width, texture_height, input_format))
   {
      viewer.render_loop(frame_rate_in_ms, [&synchronisation_cv, &synchronisation_mutex]() {
         std::unique_lock<std::mutex> lk(synchronisation_mutex);
         synchronisation_cv.wait(lk);
      });
      viewer.release();
   }
   else
      std::cout << "VideoViewer initialization failed" << std::endl;
   
   stop.store(true);
}
#endif

void handle_key(Deltacast::VideoViewer& viewer, std::atomic<bool>& stop)
{
   init_keyboard();

   std::cout << "Press any key to stop or close Opengl window..." << std::endl;
   while (!viewer.window_request_close() && !stop.load())
   {
      if (_kbhit())
      {
         _getch();
         break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
   }
   
   stop.store(true);
   close_keyboard();
}

int main(int argc, char** argv) 
{
   std::atomic<bool> stop(false);
   std::condition_variable synchronisation_cv;
   std::mutex synchronisation_mutex;
   Deltacast::VideoViewer viewer;

   int texture_width = 720;
   int texture_height = 576;

#if !defined(__APPLE__)
   //Starting VideoViewer rendering inside a new thread   
   std::thread viewerthread(render_video, std::ref(viewer), 800, 600, "My window", texture_width, texture_height, Deltacast::VideoViewer::InputFormat::ycbcr_422_10_le_msb, 10, std::ref(stop), std::ref(synchronisation_cv), std::ref(synchronisation_mutex));
#else
   if(! viewer.init(800, 600, "My window", texture_width, texture_height, Deltacast::VideoViewer::InputFormat::bgr_444_8_le_msb))
   {
      std::cout << "VideoViewer initialization failed" << std::endl;
      return -1;
   }
#endif

   //Starting Pattern Generator inside a new thread
   std::thread pattern_generator(pattern_thread, std::ref(viewer), texture_width, texture_height, Deltacast::ColorBar::PixelFormat::ycbcr_422_10_le_msb, std::ref(stop), std::ref(synchronisation_cv), std::ref(synchronisation_mutex));

   std::thread handle_key_thread(handle_key, std::ref(viewer), std::ref(stop));

#if defined(__APPLE__)
   viewer.start();
   while(!viewer.window_request_close() && !stop.load())
   {
      auto start_time = std::chrono::high_resolution_clock::now();
      {
         std::unique_lock<std::mutex> lk(synchronisation_mutex);
         synchronisation_cv.wait(lk);
      }
      viewer.process_escape_key();
      viewer.render_iteration();
      auto elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
      std::this_thread::sleep_for(frame_rate_in_ms - elapsed_time);
   }
   stop.store(true);
   viewer.release();
#endif

   //Stopping Pattern Generator
   handle_key_thread.join();
   pattern_generator.join();

   //Stopping VideoViewer
   viewer.stop();
#if !defined(__APPLE__)
   viewerthread.join();
#endif

   return 0;
} 