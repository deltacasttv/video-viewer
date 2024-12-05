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
#include "videoviewer_internal.hpp"

Deltacast::VideoViewer_Internal gl_video_viewer;

Deltacast::VideoViewer::VideoViewer()
{
}
Deltacast::VideoViewer::~VideoViewer()
{
}

bool Deltacast::VideoViewer::init(int window_width, int window_height, const char* window_title, int texture_width, int texture_height, InputFormat input_format)
{
   if (gl_video_viewer.create_window(window_width, window_height, window_title))
   {
      return gl_video_viewer.init(texture_width, texture_height, input_format);
   }

   return false;
}

bool Deltacast::VideoViewer::init(int window_width, int window_height, const char* window_title, int texture_width,
   int texture_height, InputFormat input_format, int xpos, int ypos)
{
   if (gl_video_viewer.create_window(window_width, window_height, window_title, xpos, ypos))
   {
      return gl_video_viewer.init(texture_width, texture_height, input_format);
   }

   return false;
}

bool Deltacast::VideoViewer::release()
{
   bool result_release = gl_video_viewer.release();
   bool result_destroy = gl_video_viewer.destroy_window();

   return result_release && result_destroy;
}

bool Deltacast::VideoViewer::render_loop(int frame_rate_in_ms, std::function<void()> sync_func)
{
   return gl_video_viewer.render_loop(frame_rate_in_ms, sync_func);
}

void Deltacast::VideoViewer::render_iteration()
{
   gl_video_viewer.render_iteration();
}

void Deltacast::VideoViewer::start()
{
   gl_video_viewer.set_rendering_active(true);
}

void Deltacast::VideoViewer::stop()
{
   gl_video_viewer.stop();
}
bool Deltacast::VideoViewer::lock_data(uint8_t** data, uint64_t* size)
{
   return gl_video_viewer.lock_data(data, size);
}
void Deltacast::VideoViewer::unlock_data()
{
   gl_video_viewer.unlock_data();
}
bool Deltacast::VideoViewer::window_request_close()
{
   return gl_video_viewer.window_request_close();
}

bool Deltacast::VideoViewer::window_set_title(const char* title)
{
   return gl_video_viewer.window_set_title(title);
}

void Deltacast::VideoViewer::process_escape_key()
{
   gl_video_viewer.process_escape_key();
}