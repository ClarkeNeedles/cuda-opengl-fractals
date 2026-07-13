/******************************************************************************
 * @file    shader.cpp
 * @author  Clarke Needles
 * @brief   OpenGL shader compilation and management.
 ******************************************************************************/

#include "shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    GLuint vertexShader = compileShader(
        GL_VERTEX_SHADER,
        vertexCode.c_str());

    GLuint fragmentShader = compileShader(
        GL_FRAGMENT_SHADER,
        fragmentCode.c_str());

    m_ID = glCreateProgram();

    glAttachShader(m_ID, vertexShader);
    glAttachShader(m_ID, fragmentShader);
    glLinkProgram(m_ID);

    GLint success;
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[1024];

        glGetProgramInfoLog(
            m_ID,
            sizeof(infoLog),
            nullptr,
            infoLog
        );

        std::cerr << "Shader linking failed:\n" 
            << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() const
{
    glUseProgram(m_ID);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(
        glGetUniformLocation(m_ID, name.c_str()),
        value
    );
}

GLuint Shader::getID() const
{
    return m_ID;
}

std::string Shader::readFile(const char* path)
{
    std::ifstream file;

    file.open(path);

    if (!file.is_open())
    {
        std::cerr
            << "Failed to open shader file: "
            << path
            << std::endl;

        return "";
    }

    std::stringstream stream;
    stream << file.rdbuf();

    file.close();

    return stream.str();
}

GLuint Shader::compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);

    glShaderSource(
        shader,
        1,
        &source,
        nullptr
    );

    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[1024];

        glGetShaderInfoLog(
            shader,
            sizeof(infoLog),
            nullptr,
            infoLog
        );

        std::cerr
            << "Shader compilation failed:\n"
            << infoLog
            << std::endl;
    }
    
    return shader;
}