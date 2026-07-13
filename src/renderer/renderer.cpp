/******************************************************************************
 * @file    renderer.cpp
 * @author  Clarke Needles
 * @brief   CUDA/OpenGL fractal rendering implementation.
 ******************************************************************************/

#include "renderer.h"

#include "../CUDA/kernel.cuh"
#include <chrono>
#include <iostream>

Renderer::Renderer(int max_iters, int width, int height) :
    m_maxIters(max_iters), m_width(width), m_height(height),
    m_shader(
        "OpenGL/shaders/fullscreen.vert",
        "OpenGL/shaders/fullscreen.frag"
    )
{
    createPBO();

    createTexture();

    createFullscreenQuad();
}

Renderer::~Renderer()
{
    if (m_cudaResource)
    {
        cudaGraphicsUnregisterResource(m_cudaResource);
    }

    if (m_pbo)
    {
        glDeleteBuffers(1, &m_pbo);
    }

    if (m_texture)
    {
        glDeleteTextures(1, &m_texture);
    }

    if (m_VBO)
    {
        glDeleteBuffers(1, &m_VBO);
    }

    if (m_VAO)
    {
        glDeleteVertexArrays(1, &m_VAO);
    }
}

void Renderer::createPBO()
{
    glGenBuffers(1, &m_pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);

    glBufferData(
        GL_PIXEL_UNPACK_BUFFER,
        m_width * m_height * sizeof(uchar4),
        nullptr,
        GL_DYNAMIC_DRAW
    );

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    cudaGraphicsGLRegisterBuffer(
        &m_cudaResource,
        m_pbo,
        cudaGraphicsMapFlagsWriteDiscard
    );
}

void Renderer::createTexture()
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8,
        m_width, m_height, 0, GL_RGBA, 
        GL_UNSIGNED_BYTE, nullptr
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::createFullscreenQuad()
{
    float vertices[] =
    {
        // position      // uv

        -1.0f,  1.0f,     0.0f, 1.0f,
        -1.0f, -1.0f,     0.0f, 0.0f,
         1.0f, -1.0f,     1.0f, 0.0f,

        -1.0f,  1.0f,     0.0f, 1.0f,
         1.0f, -1.0f,     1.0f, 0.0f,
         1.0f,  1.0f,     1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE, 
        4 * sizeof(float), (void*)0
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(float), (void*)(2 * sizeof(float))
    );

    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Renderer::render()
{
    uchar4* d_output;
    size_t size;
    auto start = std::chrono::high_resolution_clock::now();

    cudaGraphicsMapResources(1, &m_cudaResource, 0);

    cudaGraphicsResourceGetMappedPointer(
        (void**)&d_output,
        &size,
        m_cudaResource
    );

    launch_mandelbrot_kernel(
        d_output,
        m_width,
        m_height,
        m_zoom,
        m_moveX,
        m_moveY,
        m_maxIters
    );

    cudaGraphicsUnmapResources(1, &m_cudaResource, 0);

    updateTexture();
    draw();

    auto end = std::chrono::high_resolution_clock::now();
    m_renderTime = std::chrono::duration<float, std::milli>(
        end - start).count();
}

void Renderer::updateTexture()
{
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0,
        0, m_width, m_height,
        GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void Renderer::draw()
{
    m_shader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    m_shader.setInt("screenTexture", 0);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Renderer::zoomIn()
{
    m_zoom *= 1.15f;
}

void Renderer::zoomOut()
{
    m_zoom /= 1.15f;
}

void Renderer::move(float dx, float dy)
{
    m_moveX += dx / m_zoom;
    m_moveY += dy / m_zoom;
}

float Renderer::getRenderTime() const
{
    return m_renderTime;
}