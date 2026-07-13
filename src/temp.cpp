/******************************************************************************
 * @file    main.cpp
 * @author  Clarke Needles
 * @brief   Main loop for the fractal generation application.
 ******************************************************************************/

#include <GL/glew.h>  
#include <GLFW/glfw3.h>
#include <iostream>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include "CUDA/kernel.cuh"

const int WIDTH = 900;
const int HEIGHT = 600;

GLuint pbo;                              // OpenGL Pixel Buffer Object ID
GLuint textureID;                        // OpenGL Texture ID
cudaGraphicsResource* cuda_pbo_resource; // Shared handle tracking the buffer

void initOpenGLAndCUDA(GLFWwindow* window) 
{
    // 1. Generate standard OpenGL Pixel Buffer Object (PBO)
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, WIDTH * HEIGHT * sizeof(uchar4), NULL, GL_DYNAMIC_DRAW);

    // 2. Register the PBO with CUDA to lock the shared memory
    cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, pbo, cudaGraphicsMapFlagsWriteDiscard);

    // 3. Create the texture that displays the canvas
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void render() 
{
    // 1. Lock the buffer so OpenGL releases control and CUDA takes over
    uchar4* d_output;
    size_t size;
    cudaGraphicsMapResources(1, &cuda_pbo_resource, 0);
    cudaGraphicsResourceGetMappedPointer((void**)&d_output, &size, cuda_pbo_resource);

    // 2. Launch your Mandelbrot Math calculations on your RTX 3070
    launch_mandelbrot_kernel(d_output, WIDTH, HEIGHT, 1.0f, -0.5f, 0.0f);

    // 3. Unlock the buffer so OpenGL can safely read the data again
    cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0);

    // 4. Bind the updated buffer into our active texture display
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // 5. Draw a simple screen-space rectangle mapped with the Mandelbrot texture
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

int main() 
{
    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "CUDA-OpenGL Fractal Explorer", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit(); // Initialize your OpenGL extensions fetcher

    initOpenGLAndCUDA(window);

    while (!glfwWindowShouldClose(window)) 
    {
        glClear(GL_COLOR_BUFFER_BIT);
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up memory leaks safely
    cudaGraphicsUnregisterResource(cuda_pbo_resource);
    glDeleteBuffers(1, &pbo);
    glDeleteTextures(1, &textureID);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
