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
#include <GL/gl3w.h>

#include <string>

class Shader
{
 public:
    Shader() = default;
    Shader(const std::string& vertex_path, const std::string& fragment_path);
    ~Shader();

    void open(const std::string& vertex_path, const std::string& fragment_path);
    void compile(const std::string& vertex_code, const std::string& fragment_code);
    void use();

    // utility uniform functions
    void set_bool(const std::string& name, bool value) const;
    void set_int(const std::string& name, int value) const;
    void set_float(const std::string& name, float value) const;

    GLuint get_program_id();
    GLuint get_uniform_location(const std::string& name);

 private:
    GLuint      program_id, vertex_shader_id, fragment_shader_id;
    std::string vertex_code;
    std::string fragment_code;

    bool check_compile_errors(GLuint shader, std::string type);
    void extract_shaders_code(const std::string& vertex_path, const std::string& fragment_path);
    void compile_shaders();
};

class ShaderException : public std::exception
{
 public:
    ShaderException(const std::string& message) : message(message) {}
    const char* what() const noexcept override { return message.c_str(); }

 private:
    std::string message;
};