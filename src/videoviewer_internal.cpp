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

#include "videoviewer_internal.hpp"
#include "shaders/vertices.glsl"
#include "shaders/render.glsl"
#include "shaders/ycbcr_422_8_to_rgb_4444.glsl"
#include "shaders/rgb_444_8_to_rgb_4444.glsl"
#include "shaders/bgr_444_8_to_rgb_4444.glsl"
#include "shaders/ycbcr_444_8_to_rgb_4444.glsl"
#include "shaders/bgr_444_8_le_msb_to_rgb_4444.glsl"
#include "shaders/ycbcr_422_10_be_to_rgb_4444.glsl"
#include "shaders/ycbcr_422_10_le_msb_to_rgb_4444.glsl"
#include "shaders/yuv420_semiplanar_to_rgb_4444.glsl"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <glm/glm.hpp>

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#include <cstring>
#endif

#define GL_CHECK(func, ...) gl_check(__FILE__, __LINE__, #func, func, ##__VA_ARGS__)
#define GLFW_CHECK(func, ...) glfw_check(__FILE__, __LINE__, #func, func, ##__VA_ARGS__)
#define GL_CHECK_OUTPUT(func, ...) gl_check_output(__FILE__, __LINE__, #func, func, ##__VA_ARGS__)
#define GLFW_CHECK_OUTPUT(func, ...) glfw_check_output(__FILE__, __LINE__, #func, func, ##__VA_ARGS__)

using namespace Deltacast;
struct Vertex
{
   glm::vec2 position;
   glm::vec2 texture_coordinate;
   glm::vec2 flip_texture_coordinate;
};

const std::vector<Vertex> vertices = {
    {{-1.0f, -1.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}}, // {triangle vertices x, y} , {texture x, y}, {flip texture x, y}
    {{1.0f, -1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}},
    {{1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}},
    {{-1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}},
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};

static uint32_t g_window_width = 0;
static uint32_t g_window_height = 0;

VideoViewer_Internal::VideoViewer_Internal()
{
}

VideoViewer_Internal::~VideoViewer_Internal()
{
   glfwTerminate();
}

void VideoViewer_Internal::PrintGLError(GLenum error, const char* stmt, const char* fname, int line)
{
   std::cout << "Opengl error: " << error << " [" << stmt  << "]" << "in file " << fname << " @ line " << line << std::endl;
}

void VideoViewer_Internal::PrintGLFWError(int code, const char* description, const char* stmt, const char* fname, int line)
{
      std::cout << "GLFW error: " << code << " [" << stmt  << "]" << "in file " << fname << " @ line " << line;

      if (description)
         std::cout << " description: " << description;

      std::cout << std::endl;
}

void VideoViewer_Internal::delete_texture()
{
   GL_CHECK(glBindTexture, GL_TEXTURE_2D, 0);
   GL_CHECK(glDeleteTextures, 1, &m_texture_from_buffer);
   GL_CHECK(glDeleteTextures, 1, &m_texture_to_render);

   if (m_texture_uv_buffer != 0)
   {
      GL_CHECK(glDeleteTextures, 1, &m_texture_uv_buffer);
      m_texture_uv_buffer = 0;
   }
}

void VideoViewer_Internal::delete_vertexes()
{
   GL_CHECK(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);
   GL_CHECK(glDeleteBuffers, 1, &m_index_buffer_object);

   GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, 0);
   GL_CHECK(glDeleteBuffers, 1, &m_vertex_buffer_object);

   GL_CHECK(glBindVertexArray, 0);

   GL_CHECK(glDeleteVertexArrays, 1, &m_render_vertex_array);
   GL_CHECK(glDeleteVertexArrays, 1, &m_conversion_vertex_array);
}

void VideoViewer_Internal::delete_framebuffers()
{
   if (m_conversion_framebuffer != 0)
   {
      GL_CHECK(glBindFramebuffer, GL_FRAMEBUFFER, 0);
      GL_CHECK(glDeleteFramebuffers, 1, &m_conversion_framebuffer);
      m_conversion_framebuffer = 0;
   }
}

void VideoViewer_Internal::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   g_window_width = width;
   g_window_height = height;
   GL_CHECK(glViewport, 0, 0, g_window_width, g_window_height);
}

bool VideoViewer_Internal::create_window(int width, int height, const char* title)
{
   return create_window(width, height, title, false);
}

bool VideoViewer_Internal::create_window(int width, int height, const char* title, const int xpos, const int ypos)
{
   bool Result = create_window(width, height, title, true);

   if(Result)
   {
      Result = window_set_position(xpos, ypos);
      GLFW_CHECK(glfwShowWindow, m_window);
   }

   return Result;
}

bool VideoViewer_Internal::create_window(int width, int height, const char* title, const bool is_invisible)
{
   const char* error_desc;
   int error_code;
   std::lock_guard<std::mutex> guard(m_rendering_mutex);

   if (!GLFW_CHECK_OUTPUT(glfwInit).value)
     return false;

   GLFW_CHECK(glfwWindowHint, GLFW_CONTEXT_VERSION_MAJOR, 4);
   // Apple supports only OpenGL 4.1 maximum
   GLFW_CHECK(glfwWindowHint, GLFW_CONTEXT_VERSION_MINOR, 1);
   GLFW_CHECK(glfwWindowHint, GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   GLFW_CHECK(glfwWindowHint, GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

   if(is_invisible)
      GLFW_CHECK(glfwWindowHint, GLFW_VISIBLE, false);

   m_window = GLFW_CHECK_OUTPUT(glfwCreateWindow, width, height, title, nullptr, nullptr).value;
   g_window_width = width;
   g_window_height = height;
   m_window_width = width;
   m_window_height = height;

   if (!m_window)
     return false;

   GLFW_CHECK(glfwMakeContextCurrent, m_window);

   if (gl3wInit() != 0)
     return false;

   GLFW_CHECK(glfwSetFramebufferSizeCallback, m_window, VideoViewer_Internal::framebuffer_size_callback);

   return true;
}

bool VideoViewer_Internal::init(int texture_width, int texture_height, Deltacast::VideoViewer::InputFormat input_format)
{
   uint64_t input_buffer_size = 0;
   const char* conversion_shader_name = "";

   if(!m_conversion_shader)
      m_conversion_shader = std::make_unique<Shader>();

   if(!m_render_shader)
      m_render_shader = std::make_unique<Shader>();

   m_texture_width = texture_width;
   m_texture_height = texture_height;
   m_input_format = input_format;
   switch(m_input_format)
   {
      case Deltacast::VideoViewer::InputFormat::ycbcr_422_8:
         if((m_texture_width % 2) != 0)
         {
            std::cout << "Texture width not supported in this format" << std::endl;
            return false;
         }
         m_internal_pixel_format = GL_RGBA8;
         m_internal_texture_width = static_cast<uint32_t>(m_texture_width / 2);
         m_internal_texture_height = m_texture_height;
         m_internal_texture_format = GL_RGBA;
         m_internal_pixel_type = GL_UNSIGNED_BYTE;
         m_is_semi_planar = false;
         input_buffer_size = static_cast<uint64_t>(m_texture_width) * static_cast<uint64_t>(m_texture_height) * 2;
         conversion_shader_name = fragment_shader_ycbcr_422_8_to_rgb_44444;
         m_data.resize(input_buffer_size);
         break;
      case Deltacast::VideoViewer::InputFormat::rgb_444_8:
         if (((m_texture_width * 3) % 4) != 0)
         {
            std::cout << "Texture width not supported in this format" << std::endl;
            return false;
         }
         m_internal_pixel_format = GL_RGB8;
         m_internal_texture_width = m_texture_width;
         m_internal_texture_height = m_texture_height;
         m_internal_texture_format = GL_RGB;
         m_internal_pixel_type = GL_UNSIGNED_BYTE;
         m_is_semi_planar = false;
         input_buffer_size = static_cast<uint64_t>(m_texture_width) * static_cast<uint64_t>(m_texture_height) * 3;
         conversion_shader_name = fragment_shader_rgb_444_8_to_rgb_44444;
         m_data.resize(input_buffer_size);
         break;
      case Deltacast::VideoViewer::InputFormat::bgr_444_8:
         if (((m_texture_width * 3) % 4) != 0)
         {
            std::cout << "Texture width not supported in this format" << std::endl;
            return false;
         }
         m_internal_pixel_format = GL_RGB8;
         m_internal_texture_width = m_texture_width;
         m_internal_texture_height = m_texture_height;
         m_internal_texture_format = GL_BGR;
         m_internal_pixel_type = GL_UNSIGNED_BYTE;
         m_is_semi_planar = false;
         input_buffer_size = static_cast<uint64_t>(m_texture_width) * static_cast<uint64_t>(m_texture_height) * 3;
         conversion_shader_name = fragment_shader_bgr_444_8_to_rgb_44444;
         m_data.resize(input_buffer_size);
         break;
      case Deltacast::VideoViewer::InputFormat::ycbcr_444_8:
         if (((m_texture_width * 3) % 4) != 0)
         {
            std::cout << "Texture width not supported in this format" << std::endl;
            return false;
         }
         m_internal_pixel_format = GL_RGB8;
         m_internal_texture_width = m_texture_width;
         m_internal_texture_height = m_texture_height;
         m_internal_texture_format = GL_RGB;
         m_internal_pixel_type = GL_UNSIGNED_BYTE;
         m_is_semi_planar = false;
         input_buffer_size = static_cast<uint64_t>(m_texture_width) * static_cast<uint64_t>(m_texture_height) * 3;
         conversion_shader_name = fragment_shader_ycbcr_444_8_to_rgb_44444;
         m_data.resize(input_buffer_size);
         break;
      case Deltacast::VideoViewer::InputFormat::bgr_444_8_le_msb:
         m_internal_pixel_format = GL_RGBA8;
         m_internal_texture_width = m_texture_width;
         m_internal_texture_height = m_texture_height;
         m_internal_texture_format = GL_RGBA;
         m_internal_pixel_type = GL_UNSIGNED_BYTE;
         m_is_semi_planar = false;
         input_buffer_size = static_cast<uint64_t>(m_texture_width) * static_cast<uint64_t>(m_texture_height) * 4;
         conversion_shader_name = fragment_shader_bgr_444_8_le_msb_to_rgb_44444;
         m_data.resize(input_buffer_size);
         break;
      case Deltacast::VideoViewer::InputFormat::ycbcr_422_10_be:
         if (((m_texture_width * m_texture_height * 5) % 2) != 0)
         {
            std::cout << "Texture ratio not supported in this format" << std::endl;
            return false;
         }

         m_internal_pixel_format = GL_RGBA8;
         m_internal_texture_width = static_cast<uint32_t>(m_texture_width * 5 / 8);
         m_internal_texture_height = m_texture_height;
         m_internal_texture_format = GL_RGBA;
         m_internal_pixel_type = GL_UNSIGNED_BYTE;
         m_is_semi_planar = false;
         input_buffer_size = static_cast<uint64_t>((m_texture_width * m_texture_height * 5) / 2);
         conversion_shader_name = fragment_shader_ycbcr_422_10_be_to_rgb_44444;
         m_data.resize(input_buffer_size);
         break;
      case Deltacast::VideoViewer::InputFormat::ycbcr_422_10_le_msb:
         if (((m_texture_width * m_texture_height * 8) % 3) != 0)
         {
            std::cout << "Texture ratio not supported in this format" << std::endl;
            return false;
         }
         if((m_texture_height % 3) != 0)
         {
            std::cout << "Texture height not supported in this format" << std::endl;
            return false;
         }

         m_internal_pixel_format = GL_RGBA8;
         m_internal_texture_width = static_cast<uint32_t>(m_texture_width * 2);
         m_internal_texture_height = static_cast<uint32_t>(m_texture_height / 3);
         m_internal_texture_format = GL_RGBA;
         m_internal_pixel_type = GL_UNSIGNED_BYTE;
         m_is_semi_planar = false;
         input_buffer_size = static_cast<uint64_t>((m_texture_width * m_texture_height * 8) / 3);
         conversion_shader_name = fragment_shader_ycbcr_422_10_le_msb_to_rgb_44444;
         m_data.resize(input_buffer_size);
         break;
      case Deltacast::VideoViewer::InputFormat::nv12:
         if ((m_texture_width % 2) != 0 || (m_texture_height % 2) != 0)
         {
            std::cout << "Texture dimensions must be even for NV12 format" << std::endl;
            return false;
         }

         m_internal_pixel_type = GL_UNSIGNED_BYTE;
         m_is_semi_planar = true;
         m_internal_pixel_format = GL_R8;
         m_internal_texture_width = m_texture_width;
         m_internal_texture_height = m_texture_height;
         m_internal_texture_format = GL_RED;
         m_uv_offset = m_internal_texture_width * m_internal_texture_height;
         m_internal_uv_texture_format = GL_RG;
         m_internal_uv_texture_pixel_format = GL_RG8;
         m_internal_uv_texture_width = m_texture_width / 2;
         m_internal_uv_texture_height = m_texture_height / 2;

         input_buffer_size = static_cast<uint64_t>(m_texture_width) * m_texture_height * 3 / 2;
         conversion_shader_name = fragment_shader_yuv420_semiplanar_to_rgb_4444;
         m_data.resize(input_buffer_size);
         break;
      case Deltacast::VideoViewer::InputFormat::p010:
         if ((m_texture_width % 2) != 0 || (m_texture_height % 2) != 0)
         {
            std::cout << "Texture dimensions must be even for P010 format" << std::endl;
            return false;
         }

         m_is_semi_planar = true;
         m_internal_pixel_format = GL_R16;
         m_internal_texture_width = m_texture_width;
         m_internal_texture_height = m_texture_height;
         m_internal_texture_format = GL_RED;
         m_internal_pixel_type = GL_UNSIGNED_SHORT;
         m_uv_offset = m_internal_texture_width * m_internal_texture_height * 2;
         m_internal_uv_texture_format = GL_RG;
         m_internal_uv_texture_pixel_format = GL_RG16;
         m_internal_uv_texture_width = m_texture_width / 2;
         m_internal_uv_texture_height = m_texture_height / 2;

         // For P010 (YUV 4:2:0 16-bit), each pixel has 1.5 samples for Y and 1.5 samples for UV.
+        // Each sample is 2 bytes (16 bits), so total bytes per pixel = (1.5 + 1.5) * 2 = 6.
         input_buffer_size = static_cast<uint64_t>(m_texture_width) * m_texture_height * 6 / 2;
         conversion_shader_name = fragment_shader_yuv420_semiplanar_to_rgb_4444;
         m_data.resize(input_buffer_size);
         break;
      default:
         return false;
   }

   std::lock_guard<std::mutex> guard(m_rendering_mutex);

   create_shaders(conversion_shader_name);
   create_textures();
   create_vertexes();
   create_framebuffers();
   return true;
}

bool VideoViewer_Internal::release()
{
   std::lock_guard<std::mutex> guard(m_rendering_mutex);

   if (m_rendering_active)
      return false;

   delete_texture();

   delete_vertexes();
   delete_framebuffers();

   m_conversion_shader.reset();
   m_render_shader.reset();

   return true;
}

void VideoViewer_Internal::create_shaders(const char* conversion_shader_name)
{
   m_conversion_shader->compile(std::string(vertex_shader_vertices), std::string(conversion_shader_name));
   m_render_shader->compile(std::string(vertex_shader_vertices), std::string(fragment_shader_render));
}

void VideoViewer_Internal::create_vertexes()
{
   GL_CHECK(glGenVertexArrays, 1, &m_render_vertex_array);
   GL_CHECK(glGenVertexArrays, 1, &m_conversion_vertex_array);
   GL_CHECK(glGenBuffers, 1, &m_vertex_buffer_object);
   GL_CHECK(glGenBuffers, 1, &m_index_buffer_object);

   GL_CHECK(glBindVertexArray, m_render_vertex_array);

   GL_CHECK(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_object);
   GL_CHECK(glBufferData, GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);

   GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, m_vertex_buffer_object);
   GL_CHECK(glBufferData, GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

   GL_CHECK(glVertexAttribPointer, 0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
   GL_CHECK(glEnableVertexAttribArray, 0);

   GL_CHECK(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_coordinate));
   GL_CHECK(glEnableVertexAttribArray, 1);

   GL_CHECK(glBindVertexArray, m_conversion_vertex_array);
   GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, m_vertex_buffer_object);
   GL_CHECK(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_object);

   GL_CHECK(glVertexAttribPointer, 0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
   GL_CHECK(glEnableVertexAttribArray, 0);

   GL_CHECK(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, flip_texture_coordinate));
   GL_CHECK(glEnableVertexAttribArray, 1);

   GL_CHECK(glBindVertexArray, 0);
}

void VideoViewer_Internal::create_framebuffers()
{
   GL_CHECK(glGenFramebuffers, 1, &m_conversion_framebuffer);
   GL_CHECK(glBindFramebuffer, GL_FRAMEBUFFER, m_conversion_framebuffer);
   GL_CHECK(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_to_render, 0);

   GLenum status = GL_CHECK_OUTPUT(glCheckFramebufferStatus, GL_FRAMEBUFFER).value;
   if(status != GL_FRAMEBUFFER_COMPLETE)
   {
      std::cout << "Framebuffer not complete" << std::endl;
      throw std::runtime_error("Framebuffer not complete");
   }

   GL_CHECK(glBindFramebuffer, GL_FRAMEBUFFER, 0);
}

void VideoViewer_Internal::create_textures()
{
   //Input texture before color conversion
   GL_CHECK(glGenTextures, 1, &m_texture_from_buffer);
   GL_CHECK(glBindTexture, GL_TEXTURE_2D, m_texture_from_buffer);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

   GL_CHECK(glTexImage2D, GL_TEXTURE_2D, 0, m_internal_pixel_format, m_texture_width, m_texture_height, 0, m_internal_texture_format, m_internal_pixel_type, nullptr);
   if(m_is_semi_planar)
   {
        GL_CHECK(glGenTextures, 1, &m_texture_uv_buffer);
        GL_CHECK(glBindTexture, GL_TEXTURE_2D, m_texture_uv_buffer);
        GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GL_CHECK(glTexImage2D, GL_TEXTURE_2D, 0, m_internal_uv_texture_pixel_format, m_internal_uv_texture_width, m_internal_uv_texture_height, 0, m_internal_uv_texture_format, m_internal_pixel_type, nullptr);
   }

   GL_CHECK(glBindTexture, GL_TEXTURE_2D, 0);

   // Output texture after color conversion in shaders
   GL_CHECK(glGenTextures, 1, &m_texture_to_render);
   GL_CHECK(glBindTexture, GL_TEXTURE_2D, m_texture_to_render);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   GL_CHECK(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA8, m_texture_width, m_texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
   GL_CHECK(glGenerateMipmap, GL_TEXTURE_2D);
   GL_CHECK(glBindTexture, GL_TEXTURE_2D, 0);
}

bool VideoViewer_Internal::window_request_close()
{
   if (m_window)
     return GLFW_CHECK_OUTPUT(glfwWindowShouldClose, m_window).value;
   else
     return false;
}

bool VideoViewer_Internal::window_set_title(const char* title)
{
   if (m_window)
   {
      GLFW_CHECK(glfwSetWindowTitle, m_window, title);
      return true;
   }

   return false;
}

bool Deltacast::VideoViewer_Internal::destroy_window()
{
   if(m_window){
      release();
      GLFW_CHECK(glfwDestroyWindow, m_window);
      return true;
   }

   return false;
}

bool VideoViewer_Internal::window_set_position(const int xpos, const int ypos)
{
   if (m_window)
   {
      GLFW_CHECK(glfwSetWindowPos, m_window, xpos, ypos);
      return true;
   }

   return false;
}

bool VideoViewer_Internal::render_loop(int frame_rate_in_ms, std::function<void()> sync_func)
{
   m_stop = false;
   m_rendering_active = true;
   auto frame_rate = std::chrono::milliseconds(frame_rate_in_ms);

   while (!GLFW_CHECK_OUTPUT(glfwWindowShouldClose, m_window).value && !m_stop)
   {
      auto start_time = std::chrono::high_resolution_clock::now();
      process_escape_key();
      sync_func();
      render_iteration();
      auto elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
      auto remaining = frame_rate - elapsed_time;
      if (remaining > std::chrono::milliseconds::zero())
         std::this_thread::sleep_for(remaining);
   }

   m_rendering_active = false;
   return true;
}

void VideoViewer_Internal::render_iteration()
{
   GLFW_CHECK(glfwPollEvents);
   render();
   GLFW_CHECK(glfwSwapBuffers, m_window);
}

void VideoViewer_Internal::stop()
{
   m_stop = true;
   m_rendering_active = false;
}

bool VideoViewer_Internal::lock_data(uint8_t** data, uint64_t* size)
{
   m_rendering_mutex.lock();

   if(m_rendering_active)
   {
      if (m_window)
      {
         if (!GLFW_CHECK_OUTPUT(glfwWindowShouldClose, m_window).value)
         {
            *size = m_data.size();

            if(m_data.size() > 0)
               *data = m_data.data();
            else
            {
               m_rendering_mutex.unlock();
               return false;
            }

            return true;
         }
      }
   }
   m_rendering_mutex.unlock();
   return false;
}

void VideoViewer_Internal::unlock_data()
{
   m_rendering_mutex.unlock();
}

void VideoViewer_Internal::render()
{
    m_window_width = g_window_width;
    m_window_height = g_window_height;
   // Bind the framebuffer for offscreen rendering
    GL_CHECK(glBindFramebuffer, GL_FRAMEBUFFER, m_conversion_framebuffer);
    GL_CHECK(glViewport, 0, 0, m_texture_width, m_texture_height);
    GL_CHECK(glBindVertexArray, m_conversion_vertex_array);
    GL_CHECK(glClear, GL_COLOR_BUFFER_BIT);

    // Use the compute shader
    m_conversion_shader->use();


    // Update the texture with new pattern data if available
    m_rendering_mutex.lock();
    if (m_data.size() > 0)
    {
        GL_CHECK(glActiveTexture, GL_TEXTURE0);
        GL_CHECK(glBindTexture, GL_TEXTURE_2D, m_texture_from_buffer);
        GL_CHECK(glTexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, m_internal_texture_width, m_internal_texture_height, m_internal_texture_format, m_internal_pixel_type, m_data.data());
        if (m_is_semi_planar)
        {
            GL_CHECK(glActiveTexture, GL_TEXTURE1);
            GL_CHECK(glBindTexture, GL_TEXTURE_2D, m_texture_uv_buffer);
            uint8_t* uv_data = m_data.data() + m_uv_offset;
            GL_CHECK(glTexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, m_internal_uv_texture_width, m_internal_uv_texture_height, m_internal_uv_texture_format, m_internal_pixel_type, uv_data);

            // Restore active texture unit to GL_TEXTURE0
            GL_CHECK(glActiveTexture, GL_TEXTURE0);
        }
    }
    m_rendering_mutex.unlock();

    // Set the texture uniform
    m_conversion_shader->set_int("texture_width", m_texture_width);
    m_conversion_shader->set_int("texture_height", m_texture_height);
    m_conversion_shader->set_int("input_texture", 0);
    if (m_is_semi_planar)
    {
        m_conversion_shader->set_int("uv_texture", 1);
    }
    m_conversion_shader->set_bool("bt_709", true);

    // Draw the triangle
    GL_CHECK(glDrawElements, GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (GLvoid *)0);

    // Copy the rendered texture to another texture
    GL_CHECK(glBindTexture, GL_TEXTURE_2D, m_texture_to_render);
    GL_CHECK(glCopyTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, m_texture_width, m_texture_height, 0);

    // Unbind the framebuffer
    GL_CHECK(glBindFramebuffer, GL_FRAMEBUFFER, 0);
    GL_CHECK(glViewport, 0, 0, m_window_width, m_window_height);
    GL_CHECK(glBindVertexArray, m_render_vertex_array);

    // Use the render shader
    m_render_shader->use();

    // Bind the texture to render
    GL_CHECK(glActiveTexture, GL_TEXTURE0);
    m_render_shader->set_int("input_texture", 0);

    // Draw the triangle
    GL_CHECK(glDrawElements, GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (GLvoid *)0);

    // Unbind all buffers and textures (not necessary but for completeness)
    GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, 0);
    GL_CHECK(glBindTexture, GL_TEXTURE_2D, 0);
    GL_CHECK(glBindVertexArray, 0);
}

void VideoViewer_Internal::process_escape_key()
{
   if (GLFW_CHECK_OUTPUT(glfwGetKey, m_window, GLFW_KEY_ESCAPE).value == GLFW_PRESS)
      GLFW_CHECK(glfwSetWindowShouldClose, m_window, true);
}