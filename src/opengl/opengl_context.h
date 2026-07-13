/******************************************************************************
 * @file    opengl_context.h
 * @author  Clarke Needles
 * @brief   Header for creation and management of the OpenGL rendering context.
 ******************************************************************************/

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class OpenGLContext
{
public:
    /**
     * @brief Creates an OpenGL window and initializes the context.
     *
     * @param width Window width.
     * @param height Window height.
     * @param title Window title.
     */
    OpenGLContext(
        int width,
        int height,
        const char* title
    );

    /**
     * @brief Cleans up the GLFW window and terminates GLFW.
     */
    ~OpenGLContext();

    /**
     * @brief Checks if the user has closed the window.
     */
    bool shouldClose() const;

    /**
     * @brief Swaps the front/back buffers.
     */
    void swapBuffers();

    /**
     * @brief Processes pending GLFW events.
     */
    void pollEvents();

    /**
     * @brief Returns the GLFW window handle.
     *
     * Useful for input callbacks.
     */
    GLFWwindow* getWindow() const;

    /**
     * @brief Returns window width.
     */
    int getWidth() const;

    /**
     * @brief Returns window height.
     */
    int getHeight() const;

private:
    GLFWwindow* m_window = nullptr;

    int m_width;
    int m_height;
};