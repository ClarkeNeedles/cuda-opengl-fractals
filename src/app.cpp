/******************************************************************************
 * @file    app.cpp
 * @author  Clarke Needles
 * @brief   Application control loop implementation.
 ******************************************************************************/

#include "App.h"

#include <GL/glew.h>
#include <iostream>

App* App::s_instance = nullptr;

App::App() :
    m_window(900, 600, "CUDA OpenGL Mandelbrot Explorer"),
    m_renderer(900, 600)
{
    s_instance = this;

    // Renderer must initialize AFTER OpenGL context exists.
    m_renderer.initialize();

    GLFWwindow* window = m_window.getWindow();
   
    // Connect GLFW callbacks.
    glfwSetMouseButtonCallback(
        window,
        mouseButtonCallback
    );

    glfwSetCursorPosCallback(
        window,
        mouseMoveCallback
    );

    glfwSetScrollCallback(
        window,
        scrollCallback
    );
}

App::~App()
{
    s_instance = nullptr;
}

void App::run()
{
    while (!m_window.shouldClose())
    {
        processInput();

        glClear(
            GL_COLOR_BUFFER_BIT
        );

        m_renderer.render();
        m_window.swapBuffers();
        m_window.pollEvents();
    }
}

void App::processInput()
{
    GLFWwindow* window = m_window.getWindow();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void App::mouseButtonCallback(
    GLFWwindow* window,
    int button,
    int action,
    int mods
)
{
    if (!s_instance)
        return;

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            s_instance->m_dragging = true;

            glfwGetCursorPos(
                window,
                &s_instance->m_lastMouseX,
                &s_instance->m_lastMouseY
            );
        }
        else if (action == GLFW_RELEASE)
        {
            s_instance->m_dragging = false;
        }
    }
}

void App::mouseMoveCallback(
    GLFWwindow* window,
    double xpos,
    double ypos
)
{
    if (!s_instance)
        return;

    if (s_instance->m_dragging)
    {
        double dx = xpos - s_instance->m_lastMouseX;
        double dy = ypos - s_instance->m_lastMouseY;

        
        // Convert pixels into fractal movement.
        // Negative X because dragging right
        // moves the world right.
        s_instance->m_renderer.move(
            static_cast<float>(-dx) * 0.002f,
            static_cast<float>(dy) * 0.002f
        );

        s_instance->m_lastMouseX = xpos;
        s_instance->m_lastMouseY = ypos;
    }
}

void App::scrollCallback(
    GLFWwindow* window,
    double xoffset,
    double yoffset
)
{
    if (!s_instance)
        return;

    if (yoffset > 0)
    {
        s_instance->m_renderer.zoomIn();
    }
    else if (yoffset < 0)
    {
        s_instance->m_renderer.zoomOut();
    }
}