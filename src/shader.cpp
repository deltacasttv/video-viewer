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

#include "shader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path)
{
    open(vertex_path, fragment_path);
}

Shader::~Shader()
{
    glDeleteProgram(program_id);
}

void Shader::extract_shaders_code(const std::string& vertex_path, const std::string& fragment_path)
{
    std::ifstream vertex_file;
    std::ifstream fragment_file;
    vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragment_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        std::stringstream vertex_shader_stream;
        vertex_file.open(vertex_path);

        if (!vertex_file.is_open())
        {
            std::cerr << "Impossible to open " << vertex_path
                      << ". Are you in the correct directory?" << std::endl;
            throw ShaderException("Impossible to open vertex shader file");
        }
        else
        {
            vertex_shader_stream << vertex_file.rdbuf();
            vertex_file.close();
            vertex_code = vertex_shader_stream.str();
            std::cout << "Load vertex code from " << vertex_path << std::endl;
        }
    }
    catch (std::ifstream::failure e)
    {
        std::cerr << "Impossible to find or read " << vertex_path
                  << ". Are you in the correct directory?" << std::endl;
        throw e;
    }

    try
    {
        std::stringstream fragment_shader_stream;
        fragment_file.open(fragment_path);
        if (!fragment_file.is_open())
        {
            std::cerr << "Impossible to open " << fragment_path
                      << ". Are you in the correct directory?" << std::endl;
            throw ShaderException("Impossible to open fragment shader file");
        }
        else
        {
            fragment_shader_stream << fragment_file.rdbuf();
            fragment_file.close();
            fragment_code = fragment_shader_stream.str();
            std::cout << "Load fragment code from " << fragment_path << std::endl;
        }
    }
    catch (std::ifstream::failure e)
    {
        std::cerr << "Impossible to find or read " << fragment_path
                  << ". Are you in the correct directory?" << std::endl;
        throw e;
    }
}

void Shader::compile_shaders()
{
    const char* vertex_code_pointer = vertex_code.c_str();
    const char* fragment_code_pointer = fragment_code.c_str();
    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_id, 1, &vertex_code_pointer, NULL);
    glCompileShader(vertex_shader_id);
    if (!check_compile_errors(vertex_shader_id, "VERTEX"))
    {
        std::cerr << "Vertex shader compilation failed" << std::endl;
        throw ShaderException("Vertex shader compilation failed");
    }

    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_id, 1, &fragment_code_pointer, NULL);
    glCompileShader(fragment_shader_id);
    if (!check_compile_errors(fragment_shader_id, "FRAGMENT"))
    {
        std::cerr << "Fragment shader compilation failed" << std::endl;
        throw ShaderException("Fragment shader compilation failed");
    }

    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);
    if (!check_compile_errors(program_id, "PROGRAM"))
    {
        std::cerr << "Program linking failed" << std::endl;
        throw ShaderException("Program linking failed");
    }

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);
}

bool Shader::check_compile_errors(GLuint shader, std::string type)
{
    GLint  success;
    GLchar info_log[512];
    if (type == "PROGRAM")
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 512, NULL, info_log);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR" << std::endl
                      << info_log << std::endl
                      << " -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, info_log);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << std::endl
                      << info_log << std::endl
                      << " -- --------------------------------------------------- -- " << std::endl;
        }
    }
    return static_cast<bool>(success);
}

void Shader::use()
{
    glUseProgram(program_id);
}

void Shader::open(const std::string& vertex_path, const std::string& fragment_path)
{
    extract_shaders_code(vertex_path, fragment_path);
    compile_shaders();
}

void Shader::compile(const std::string& vertex_code, const std::string& fragment_code)
{
    this->vertex_code = vertex_code;
    this->fragment_code = fragment_code;
    compile_shaders();
}

void Shader::set_bool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(program_id, name.c_str()), (int)value);
}

void Shader::set_int(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(program_id, name.c_str()), (int)value);
}

void Shader::set_float(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(program_id, name.c_str()), value);
}

GLuint Shader::get_program_id()
{
    return program_id;
}

GLuint Shader::get_uniform_location(const std::string& name)
{
    return glGetUniformLocation(program_id, name.c_str());
}
