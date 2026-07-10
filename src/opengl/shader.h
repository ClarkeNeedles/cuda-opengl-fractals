/******************************************************************************
 * @file    shader.h
 * @author  Clarke Needles
 * @brief   OpenGL shader program abstraction.  
 ******************************************************************************/

#pragma once

#include <GL/glew.h>

#include <string>


class Shader
{
public:

    /**
     * @brief Creates an OpenGL shader program.
     *
     * @param vertexPath Path to vertex shader source file.
     * @param fragmentPath Path to fragment shader source file.
     */
    Shader(
        const char* vertexPath,
        const char* fragmentPath
    );


    /**
     * @brief Activates this shader program.
     */
    void use() const;


    /**
     * @brief Sets an integer uniform value.
     *
     * @param name Uniform variable name.
     * @param value Integer value.
     */
    void setInt(
        const std::string& name,
        int value
    ) const;


    /**
     * @brief Returns the OpenGL shader program ID.
     */
    GLuint getID() const;


private:

    GLuint m_ID;


    std::string readFile(
        const char* path
    );


    GLuint compileShader(
        GLenum type,
        const char* source
    );
};