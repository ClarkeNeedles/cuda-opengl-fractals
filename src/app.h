/******************************************************************************
 * @file    app.h
 * @author  Clarke Needles
 * @brief   Header for main application controller.
 ******************************************************************************/

#pragma once

#include "opengl/opengl_context.h"
#include "renderer/renderer.h"

class App
{
public:
    App(int max_iters, 
        int width, 
        int height
    );

    ~App();

    void run();

private:
    /**
     * @brief Handles keyboard and mouse input.
     */
    void processInput();

    /**
     * @brief GLFW mouse button callback.
     */
    static void mouseButtonCallback(
        GLFWwindow* window,
        int button,
        int action,
        int mods
    );

    /**
     * @brief GLFW mouse movement callback.
     */
    static void mouseMoveCallback(
        GLFWwindow* window,
        double xpos,
        double ypos
    );

    /**
     * @brief GLFW scroll callback.
     */
    static void scrollCallback(
        GLFWwindow* window,
        double xoffset,
        double yoffset
    );

private:
    static App* s_instance;

    OpenGLContext m_window;
    Renderer m_renderer;

    // Mouse state
    bool m_dragging = false;
    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
};