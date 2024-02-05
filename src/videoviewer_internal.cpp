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
#include "ycbcr422_8_to_rgb444.shader"
#include "ycbcr422_10_le_msb_to_rgb444.shader"
#include "ycbcr422_10_be_to_rgb444.shader"
#include "ycbcr444_8_to_rgb444.shader"
#include "rgb444_8_to_rgb4444.shader"
#include "bgr444_8_to_rgb4444.shader"
#include "vertex.shader"
#include "fragment.shader"
#include <iostream>

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#include <string.h>
#endif

#if defined(WIN32) || defined(_WIN32)
#define SLEEP(frame_rate_in_ms) Sleep(frame_rate_in_ms);
#else
#define SLEEP(frame_rate_in_ms) usleep(frame_rate_in_ms * 1000);
#endif

#define GL_CHECK(func, ...) gl_check(__FILE__, __LINE__, #func, func, ##__VA_ARGS__)
#define GLFW_CHECK(func, ...) glfw_check(__FILE__, __LINE__, #func, func, ##__VA_ARGS__)
#define GL_CHECK_OUTPUT(func, ...) gl_check_output(__FILE__, __LINE__, #func, func, ##__VA_ARGS__)
#define GLFW_CHECK_OUTPUT(func, ...) glfw_check_output(__FILE__, __LINE__, #func, func, ##__VA_ARGS__)

using namespace Deltacast;

struct CoordinatesSet {
   GLfloat x; //vertex coordinate
   GLfloat y; //vertex coordinate
   GLfloat s; //texture coordinate
   GLfloat t; //texture coordinate
};

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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
   GL_CHECK(glDeleteTextures, 1, &m_texture_in);
   GL_CHECK(glDeleteTextures, 1, &m_texture_out);
}

void VideoViewer_Internal::delete_pixel_buffer_object()
{
   GL_CHECK(glBindBuffer, GL_PIXEL_UNPACK_BUFFER, 0);
   GL_CHECK(glDeleteBuffers, 2, m_pbo_ids);
}

void VideoViewer_Internal::delete_vertexes()
{
   GL_CHECK(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);
   GL_CHECK(glDeleteBuffers, 1, &m_index_buffer_object);

   GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, 0);
   GL_CHECK(glDeleteBuffers, 1, &m_vertex_buffer_object);

   GL_CHECK(glBindVertexArray, 0);
   GL_CHECK(glDeleteVertexArrays, 1, &m_vertex_array);
}

void VideoViewer_Internal::delete_shaders()
{
   GL_CHECK(glDeleteProgram, m_programID);
   GL_CHECK(glDeleteProgram, m_cs_program_id);
}


void VideoViewer_Internal::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   GL_CHECK(glViewport, 0, 0, width, height);
}

void VideoViewer_Internal::compileShader(GLuint shaderID, const char* sourcePointer) {
   GL_CHECK(glShaderSource, shaderID, 1, &sourcePointer, nullptr);
   GL_CHECK(glCompileShader, shaderID);

   GLint Result = GL_FALSE;
   int InfoLogLength = 1024;
   char shaderErrorMessage[1024] = { 0 };

   GL_CHECK(glGetShaderiv, shaderID, GL_COMPILE_STATUS, &Result);

   GL_CHECK(glGetShaderInfoLog, shaderID, InfoLogLength, nullptr, shaderErrorMessage);
   if (strlen(shaderErrorMessage) != 0)
      std::cout << shaderErrorMessage << std::endl;
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
   GLFW_CHECK(glfwWindowHint, GLFW_CONTEXT_VERSION_MINOR, 3);

   if(is_invisible)
      GLFW_CHECK(glfwWindowHint, GLFW_VISIBLE, false);

   m_window = GLFW_CHECK_OUTPUT(glfwCreateWindow, width, height, title, nullptr, nullptr).value;

   if (!m_window)
     return false;

   GLFW_CHECK(glfwMakeContextCurrent, m_window);

   if (gl3wInit() != 0)
     return false;

   GLFW_CHECK(glfwSetFramebufferSizeCallback, m_window, framebuffer_size_callback);

   GLFW_CHECK(glfwMakeContextCurrent, nullptr);

   return true;
}

bool VideoViewer_Internal::init(int texture_width, int texture_height, Deltacast::VideoViewer::InputFormat input_format)
{
   uint64_t input_buffer_size = 0;
   const char* compute_shader_name = "";

   m_texture_width = texture_width;
   m_texture_height = texture_height;
   m_input_format = input_format;
   switch (m_input_format)
   {
   case Deltacast::VideoViewer::InputFormat::ycbcr_422_8:
      input_buffer_size = (uint64_t) m_texture_width * (uint64_t) m_texture_height * 2;
      if ((m_texture_width % 2) != 0)
      {
        std::cout << "Texture width not supproted in this format" << std::endl;
        return false;
      }
      m_input_texture_width = m_texture_width / 2;
      m_input_texture_height = m_texture_height;
      compute_shader_name = compute_shader_422_8;
      break;
   case Deltacast::VideoViewer::InputFormat::ycbcr_422_10_le_msb:
     if (((m_texture_width * m_texture_height * 8) % 3) != 0)
     {
       std::cout << "Texture ratio not supproted in this format" << std::endl;
       return false;
     }
     if((m_texture_height % 3) != 0)
     {
       std::cout << "Texture height not supproted in this format" << std::endl;
       return false;
     }
     input_buffer_size = (uint64_t) m_texture_width * (uint64_t) m_texture_height * 8 / 3;
     m_input_texture_width = m_texture_width * 2;
     m_input_texture_height = m_texture_height / 3;
     compute_shader_name = compute_shader_422_10_le_msb;
     break;
   case Deltacast::VideoViewer::InputFormat::ycbcr_444_8:
     input_buffer_size = (uint64_t)m_texture_width * (uint64_t)m_texture_height * 3;
     if (((m_texture_width*3) % 4) != 0)
     {
       std::cout << "Texture width not supproted in this format" << std::endl;
       return false;
     }
     m_input_texture_width = m_texture_width * 3 / 4;
     m_input_texture_height = m_texture_height;
     compute_shader_name = compute_shader_yuv_444_8;
     break;
   case Deltacast::VideoViewer::InputFormat::rgb_444_8:
     input_buffer_size = (uint64_t)m_texture_width * (uint64_t)m_texture_height * 3;
     if (((m_texture_width*3) % 4) != 0)
     {
       std::cout << "Texture width not supproted in this format" << std::endl;
       return false;
     }
     m_input_texture_width = m_texture_width * 3 / 4;
     m_input_texture_height = m_texture_height;
     compute_shader_name = compute_shader_rgb_444_8;
     break;
   case Deltacast::VideoViewer::InputFormat::bgr_444_8:
     input_buffer_size = (uint64_t)m_texture_width * (uint64_t)m_texture_height * 3;
     if (((m_texture_width*3) % 4) != 0)
     {
       std::cout << "Texture width not supproted in this format" << std::endl;
       return false;
     }
     m_input_texture_width = m_texture_width * 3 / 4;
     m_input_texture_height = m_texture_height;
     compute_shader_name = compute_shader_bgr_444_8;
     break;
   case Deltacast::VideoViewer::InputFormat::ycbcr_422_10_be:
     if (((m_texture_width * m_texture_height * 5) % 2) != 0)
     {
       std::cout << "Texture ratio not supproted in this format" << std::endl;
       return false;
     }
     input_buffer_size = (uint64_t)m_texture_width * (uint64_t)m_texture_height * 5 / 2;
     m_input_texture_width = m_texture_width * 5 / 8;
     m_input_texture_height = m_texture_height;
     compute_shader_name = compute_shader_422_10_be;
     break;
   default: return false;
   }

   std::lock_guard<std::mutex> guard(m_rendering_mutex);

   GLFW_CHECK(glfwMakeContextCurrent, m_window);

   create_pixel_buffer_objects(input_buffer_size);
   create_textures(m_input_texture_width, m_input_texture_height, m_texture_width, m_texture_height);
   create_vertexes();
   create_shaders(compute_shader_name);
   GLFW_CHECK(glfwMakeContextCurrent, nullptr);

   return true;
}

bool VideoViewer_Internal::release()
{
   std::lock_guard<std::mutex> guard(m_rendering_mutex);

   if (m_rendering_active)
      return false;

   GLFW_CHECK(glfwMakeContextCurrent, m_window);

   delete_pixel_buffer_object();

   delete_texture();

   delete_vertexes();

   delete_shaders();

   GLFW_CHECK(glfwMakeContextCurrent, nullptr);

   return true;
}

void VideoViewer_Internal::create_shaders(const char* compute_shader_name)
{
   GLuint compute_shader_id;
   char* compute_shader = 0;

   GLint result = GL_FALSE;
   int info_log_length = 1024;
   char program_error_msg[1024] = { 0 };

   compute_shader_id = glCreateShader(GL_COMPUTE_SHADER);


   compileShader(compute_shader_id, compute_shader_name);

   m_cs_program_id = glCreateProgram();

   GL_CHECK(glAttachShader, m_cs_program_id, compute_shader_id);
   GL_CHECK(glLinkProgram, m_cs_program_id);

   GL_CHECK(glDeleteShader, compute_shader_id);

   GLuint vertex_shader_id;
   GLuint fragment_shader_id;

   result = GL_FALSE;
   info_log_length = 1024;
   memset(program_error_msg, 0, 1024);

   char* vertexShader = 0;
   char* fragmentShader = 0;

   vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
   fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

   compileShader(vertex_shader_id, vertex_shader_src);
   compileShader(fragment_shader_id, fragment_shader_src);

   m_programID = glCreateProgram();

   GL_CHECK(glAttachShader, m_programID, vertex_shader_id);
   GL_CHECK(glAttachShader, m_programID, fragment_shader_id);

   GL_CHECK(glLinkProgram, m_programID);

   GL_CHECK(glDeleteShader, vertex_shader_id);
   GL_CHECK(glDeleteShader, fragment_shader_id);

   GL_CHECK(glGetProgramiv, m_programID, GL_LINK_STATUS, &result);
   GL_CHECK(glGetProgramiv, m_programID, GL_INFO_LOG_LENGTH, &info_log_length);
   GL_CHECK(glGetProgramInfoLog, m_programID, info_log_length, nullptr, &program_error_msg[0]);

   if (strlen(program_error_msg) != 0)
      std::cout << program_error_msg << "\n";
}

void VideoViewer_Internal::create_vertexes()
{
   CoordinatesSet quad_vertex[4];
   unsigned short int quad_index[4];

   quad_vertex[0].x = -1.0f;
   quad_vertex[0].y = 1.0f;

   quad_vertex[1].x = -1.0f;
   quad_vertex[1].y = -1.0f;

   quad_vertex[2].x = 1.0f;
   quad_vertex[2].y = 1.0f;

   quad_vertex[3].x = 1.0f;
   quad_vertex[3].y = -1.0f;

   quad_index[0] = 0;
   quad_index[1] = 1;
   quad_index[2] = 2;
   quad_index[3] = 3;

   quad_vertex[0].s = 0.0f;
   quad_vertex[0].t = 0.0f;
   quad_vertex[1].s = 0.0f;
   quad_vertex[1].t = 1.0f;
   quad_vertex[2].s = 1.0f;
   quad_vertex[2].t = 0.0f;
   quad_vertex[3].s = 1.0f;
   quad_vertex[3].t = 1.0f;


   GL_CHECK(glGenBuffers, 1, &m_index_buffer_object);
   GL_CHECK(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_object);
   GL_CHECK(glBufferData, GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(unsigned short int), quad_index, GL_STATIC_DRAW);
   GL_CHECK(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);

   GL_CHECK(glGenBuffers, 1, &m_vertex_buffer_object);
   GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, m_vertex_buffer_object);
   GL_CHECK(glBufferData, GL_ARRAY_BUFFER, sizeof(CoordinatesSet) * 4, nullptr, GL_STATIC_DRAW);
   GL_CHECK(glBufferSubData, GL_ARRAY_BUFFER, 0, sizeof(CoordinatesSet) * 4, quad_vertex);
   GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, 0);

   GL_CHECK(glGenVertexArrays, 1, &m_vertex_array);
   GL_CHECK(glBindVertexArray, m_vertex_array);

   GL_CHECK(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_object);

   GL_CHECK(glBindBuffer, GL_ARRAY_BUFFER, m_vertex_buffer_object);

   GL_CHECK(glVertexAttribPointer, 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
   GL_CHECK(glEnableVertexAttribArray, 0);

   GL_CHECK(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 2));
   GL_CHECK(glEnableVertexAttribArray, 1);

   GL_CHECK(glBindVertexArray, 0);
}

void VideoViewer_Internal::create_textures(int input_width, int input_height, int output_width, int output_height)
{
   /* Texture d'entr�e avant conversion de couleur */
   GL_CHECK(glGenTextures, 1, &m_texture_in);
   GL_CHECK(glBindTexture, GL_TEXTURE_2D, m_texture_in);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   GL_CHECK(glGenerateMipmap, GL_TEXTURE_2D);
   GL_CHECK(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA8, input_width, input_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
   GL_CHECK(glBindTexture, GL_TEXTURE_2D, 0);

   /* Texture de sortie pour rendu */
   GL_CHECK(glGenTextures, 1, &m_texture_out);
   GL_CHECK(glBindTexture, GL_TEXTURE_2D, m_texture_out);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   GL_CHECK(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   GL_CHECK(glGenerateMipmap, GL_TEXTURE_2D);
   GL_CHECK(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGBA8, output_width, output_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
   GL_CHECK(glBindTexture, GL_TEXTURE_2D, 0);
}

void VideoViewer_Internal::create_pixel_buffer_objects(uint64_t size)
{
   m_pbo_size = size;
   GL_CHECK(glGenBuffers, 2, m_pbo_ids);
   GL_CHECK(glBindBuffer, GL_PIXEL_UNPACK_BUFFER, m_pbo_ids[0]);
   GL_CHECK(glBufferData, GL_PIXEL_UNPACK_BUFFER, size, nullptr, GL_STREAM_DRAW);
   GL_CHECK(glBindBuffer, GL_PIXEL_UNPACK_BUFFER, m_pbo_ids[1]);
   GL_CHECK(glBufferData, GL_PIXEL_UNPACK_BUFFER, size, nullptr, GL_STREAM_DRAW);
   GL_CHECK(glBindBuffer, GL_PIXEL_UNPACK_BUFFER, 0);
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

bool VideoViewer_Internal::render_loop(int frame_rate_in_ms)
{
   m_stop = false;
   m_rendering_active = true;

   while (!GLFW_CHECK_OUTPUT(glfwWindowShouldClose, m_window).value && !m_stop){
      m_rendering_mutex.lock();
      GLFW_CHECK(glfwMakeContextCurrent, m_window);

      GLFW_CHECK(glfwPollEvents);
      render();
      GLFW_CHECK(glfwSwapBuffers, m_window);

      GLFW_CHECK(glfwMakeContextCurrent, nullptr);
      m_rendering_mutex.unlock();

      SLEEP(frame_rate_in_ms);
   }

   m_rendering_active = false;
   return true;
}

void VideoViewer_Internal::stop()
{
   m_stop = true;
}

bool VideoViewer_Internal::lock_data(uint8_t** data, uint64_t* size)
{
   m_rendering_mutex.lock();

   if (m_window)
   {
      if (!GLFW_CHECK_OUTPUT(glfwWindowShouldClose, m_window).value)
      {
         *size = m_pbo_size;

         m_pbo_index = (m_pbo_index + 1) % 2;
         m_pbo_next_index = (m_pbo_index + 1) % 2;

         GLFW_CHECK(glfwMakeContextCurrent, m_window);
         GL_CHECK(glBindBuffer, GL_PIXEL_UNPACK_BUFFER, m_pbo_ids[m_pbo_next_index]);
         GL_CHECK(glBufferData, GL_PIXEL_UNPACK_BUFFER, m_pbo_size, nullptr, GL_STREAM_DRAW);
         *data = (uint8_t*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

         return true;
      }
   }
   m_rendering_mutex.unlock();
   return false;
}

void VideoViewer_Internal::unlock_data()
{
   GL_CHECK(glUnmapBuffer, GL_PIXEL_UNPACK_BUFFER);
   GL_CHECK(glBindBuffer, GL_PIXEL_UNPACK_BUFFER, 0);
   GLFW_CHECK(glfwMakeContextCurrent, nullptr);
   m_rendering_mutex.unlock();
}

void VideoViewer_Internal::render()
{
   run_compute_shader(m_texture_width /8, m_texture_height /8, m_input_texture_width, m_input_texture_height);

   GL_CHECK(glEnable, GL_CULL_FACE);

   GL_CHECK(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   GL_CHECK(glUseProgram, m_programID);

   GL_CHECK(glBindVertexArray, m_vertex_array);

   GL_CHECK(glActiveTexture, GL_TEXTURE0);
   GL_CHECK(glBindTexture, GL_TEXTURE_2D, m_texture_out);

   GL_CHECK(glDrawElements, GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (GLvoid*)0);

   GL_CHECK(glBindTexture, GL_TEXTURE_2D, 0);
   //GL_CHECK(glActiveTexture(0));

   GL_CHECK(glBindVertexArray, 0);

   GL_CHECK(glUseProgram, 0);
}

bool VideoViewer_Internal::run_compute_shader(GLuint num_groups_x, GLuint num_groups_y, int input_width, int input_height)
{
   int workgroup_count[2] = { 0,0 };
   GL_CHECK(glGetIntegeri_v, GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workgroup_count[0]);
   GL_CHECK(glGetIntegeri_v, GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workgroup_count[1]);

   if ((int)num_groups_x > workgroup_count[0] || (int)num_groups_y > workgroup_count[1])
   {
      std::cout << "Error: " << num_groups_x << " groups_x [max " << workgroup_count[0] << "]" << std::endl;
      std::cout << "Error: " << num_groups_y << " groups_y [max " << workgroup_count[1] << "]" << std::endl;

      return false;
   }

   int workgroup_size[2] = { 0,0 };

   GL_CHECK(glGetIntegeri_v, GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workgroup_size[0]);
   GL_CHECK(glGetIntegeri_v, GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workgroup_size[1]);

   if ((int)(m_texture_width /num_groups_x) > workgroup_size[0] || (int)(m_texture_height / num_groups_y) > workgroup_size[1])
   {
      std::cout << "Error: group_x size = " << m_texture_width / num_groups_x << " [max " << workgroup_size[0] << "]" << std::endl;
      std::cout << "Error: group_y size = " << m_texture_height / num_groups_y << " [max " << workgroup_size[1] << "]" << std::endl;

      return false;
   }

   GL_CHECK(glUseProgram, m_cs_program_id);

   GL_CHECK(glBindTexture, GL_TEXTURE_2D, m_texture_out);
   GL_CHECK(glBindImageTexture, 0, m_texture_out, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

   GL_CHECK(glBindTexture, GL_TEXTURE_2D, m_texture_in);
   GL_CHECK(glBindBuffer, GL_PIXEL_UNPACK_BUFFER, m_pbo_ids[m_pbo_index]);
   GL_CHECK(glTexSubImage2D, GL_TEXTURE_2D, 0, 0, 0, input_width, input_height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
   GL_CHECK(glBindBuffer, GL_PIXEL_UNPACK_BUFFER, 0);
   GL_CHECK(glBindImageTexture, 1, m_texture_in, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);

   GL_CHECK(glDispatchCompute, num_groups_x, num_groups_y, 1);

   GL_CHECK(glMemoryBarrier, GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

   GL_CHECK(glBindImageTexture, 1, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
   GL_CHECK(glBindImageTexture, 0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
   GL_CHECK(glBindTexture, GL_TEXTURE_2D, 0);

   GL_CHECK(glUseProgram, 0);

   return true;
}