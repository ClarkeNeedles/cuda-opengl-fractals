/******************************************************************************
 * @file    mandelbrot.h
 * @author  Clarke Needles
 * @brief   Header file for CUDA/OpenGL Mandelbrot rendering.
 ******************************************************************************/

#pragma once

#include <GL/glew.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include "../OpenGL/Shader.h"

class MandelbrotRenderer
{
public:

    /**
     * @brief Creates the Mandelbrot renderer.
     *
     * @param width  Rendering width.
     * @param height Rendering height.
     */
    MandelbrotRenderer(
        int width,
        int height
    );


    /**
     * @brief Releases CUDA and OpenGL resources.
     */
    ~MandelbrotRenderer();


    /**
     * @brief Initializes OpenGL/CUDA resources.
     */
    void initialize();


    /**
     * @brief Generates and displays one Mandelbrot frame.
     */
    void render();


    // Camera controls

    void zoomIn();

    void zoomOut();

    void move(
        float dx,
        float dy
    );


    /**
     * @brief Returns the last CUDA render time in milliseconds.
     */
    float getRenderTime() const;


private:


    void createPBO();

    void createTexture();

    void createFullscreenQuad();

    void updateTexture();

    void draw();


private:


    int m_width;
    int m_height;


    // CUDA/OpenGL shared buffer

    GLuint m_pbo = 0;

    cudaGraphicsResource* m_cudaResource = nullptr;


    // OpenGL texture

    GLuint m_texture = 0;


    // Fullscreen quad

    GLuint m_VAO = 0;

    GLuint m_VBO = 0;


    // Shader

    Shader m_shader;


    // Mandelbrot parameters

    float m_zoom = 1.0f;

    float m_moveX = -0.5f;

    float m_moveY = 0.0f;


    int m_maxIterations = 1000;


    // Timing

    float m_renderTime = 0.0f;
};