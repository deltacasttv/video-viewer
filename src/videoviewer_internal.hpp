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

#include <mutex>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <videoviewer/videoviewer.hpp>
#include <vector>
#include <memory>
#include <functional>
#include "shader.hpp"


namespace Deltacast
{
   class VideoViewer_Internal
   {
   public:

      VideoViewer_Internal();
      ~VideoViewer_Internal();

      bool create_window(int width, int height, const char* title);
      bool create_window(int width, int height, const char* title, const int xpos, const int ypos);
      bool init(int texture_width, int texture_height, Deltacast::VideoViewer::InputFormat input_format);
      bool release();
      bool render_loop(int frame_rate_in_ms, std::function<void()> sync_func = [](){});
      void render_iteration();
      void stop();
      bool lock_data(uint8_t** data, uint64_t* size);
      void unlock_data();
      bool window_request_close();
      bool window_set_title(const char* title);
      bool destroy_window();
      void process_escape_key();

      void set_rendering_active(bool rendering_active) { m_rendering_active = rendering_active; }
      bool is_rendering_active() { return m_rendering_active; }
      void set_stop(bool stop) { m_stop = stop; }
      bool is_stop() { return m_stop; }

   private:
      bool create_window(int width, int height, const char* title, const bool is_invisible);
      bool window_set_position(const int xpos, const int ypos);
      void render();
      void create_vertexes();
      void create_textures();
      void create_framebuffers();
      void create_shaders(const char* conversion_shader_name);
      void delete_texture();
      void delete_vertexes();
      void delete_framebuffers();

      static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
      static void PrintGLError(GLenum error, const char* stmt, const char* fname, int line);
      static void PrintGLFWError(int code, const char* description, const char* stmt, const char* fname, int line);

      GLFWwindow* m_window{nullptr};
      GLuint m_texture_from_buffer{0};
      GLuint m_texture_to_render{0};
      GLuint m_conversion_framebuffer{0};
      GLuint m_conversion_vertex_array{0};
      GLuint m_render_vertex_array{0};
      GLuint m_index_buffer_object{0};
      GLuint m_vertex_buffer_object{0};
      uint32_t m_window_width{0};
      uint32_t m_window_height{0};
      uint32_t m_texture_width{0};
      uint32_t m_texture_height{0};
      uint16_t m_internal_pixel_format{GL_RGBA8};
      uint32_t m_internal_texture_width{0};
      uint32_t m_internal_texture_height{0};
      uint16_t m_internal_texture_format{GL_RGBA};


      std::mutex m_rendering_mutex;
      Deltacast::VideoViewer::InputFormat m_input_format{Deltacast::VideoViewer::InputFormat::nb_input_format};
      bool m_stop{false};
      bool m_rendering_active{false};
      std::mutex m_data_mutex;
      std::vector<uint8_t> m_data;      

      std::unique_ptr<Shader> m_conversion_shader;
      std::unique_ptr<Shader> m_render_shader;

      class Result {
      public:
         bool success = true;
         operator bool() const { return success; }
      };

      template<typename T>
      class ResultwValue : public Result{
      public:
         ResultwValue() = default;
         T value;
      };

      template<typename Func, typename... Args>
      static Result gl_check(const char* filename, int line, const char* funcname, Func func, Args... args) {
         GLenum err;
         Result result;

         func(args...);
         err = glGetError();
         if (err != GL_NO_ERROR)
         {
            result.success = false;
            #ifdef _DEBUG
            PrintGLError(err, funcname, filename, line);
            #endif
         }
         return result;
      }

      template<typename Func, typename... Args>
      static auto gl_check_output(const char* filename, int line, const char* funcname, Func func, Args... args)
         -> ResultwValue<decltype(func(args...))> {

         GLenum err;
         ResultwValue<decltype(func(args...))> result;

         result.value = func(args...);
         err = glGetError();
         if (err != GL_NO_ERROR)
         {
            result.success = false;
            #ifdef _DEBUG
            PrintGLError(err, funcname, filename, line);
            #endif
         }
         return result;
      }

      template<typename Func, typename... Args>
      static Result glfw_check(const char* filename, int line, const char* funcname, Func func, Args... args) {
         int code;
         const char* description;
         Result result;

         func(args...);
         code = glfwGetError(&description);
         if(code != GLFW_NO_ERROR)
         {
            result.success = false;
            #ifdef _DEBUG
            PrintGLFWError(code, description, funcname, filename, line);
            #endif
         }
         return result;
      }

      template<typename Func, typename... Args>
      static auto glfw_check_output(const char* filename, int line, const char* funcname, Func func, Args... args)
         -> ResultwValue<decltype(func(args...))> {

         int code;
         const char* description;
         ResultwValue<decltype(func(args...))> result;

         result.value = func(args...);
         code = glfwGetError(&description);
         if(code != GLFW_NO_ERROR)
         {
            result.success = false;
            #ifdef _DEBUG
            PrintGLFWError(code, description, funcname, filename, line);
            #endif
         }
         return result;
      }

   };
}