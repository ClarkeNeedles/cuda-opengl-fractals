---
title: "Code refactor"
date: 2026-07-10
status: in-progress
version: 0.2.1
---

begin refactoring the code so that it is far more modular
leaves the ability to add more types of fractals in the future
still need to comment a lot of the code
code is still not building, continue to refactor until you get a working building

I had an idea to essentially have a config.h file that contains all of the constant definitions related to the app
max iterations
screen width
screen height

include the config.h file in the main.cpp and then just pass it into the app constructor and it will handle everything

benchmarking the sister repo https://github.com/ClarkeNeedles/FractalGenerator to get the generation times on CPU
This is for a 900x600 window running on a Processor	11th Gen Intel(R) Core(TM) i7-1165G7 @ 2.80GHz, 2803 Mhz, 4 Core(s), 8 Logical Processor(s)

5 runs each
C++ --> 1766.80 - 1854.21 ms
SSE --> 461.98 - 503.35 ms
AVX --> 239.81 - 259.39 ms
C++ & Multithreading --> 767.21 - 804.78 ms
SSE & Multithreading --> 196.09 - 214.47 ms
AVX & Multithreading --> 107.37 - 111.56 ms

moving the initialization function into the renderer constructor
this is safe because m_window the OpenGLContext window is properly initialized before
making the opengl specific function calls in m_renderer constructor
if you even modify the App constructor, you must initialize the m_window before m_renderer always
create the context and then use the function pointers once they are active

why do we use shaders?

## 🎨 The Purpose of Shaders in Modern OpenGL
Modern OpenGL uses a strict programmable pipeline. The graphics card does not inherently know how to paint shapes or display raw pixel arrays. You must supply two specialized shaders to get anything onto the screen:

   1. Vertex Shader: Calculates where shapes sit in 2D or 3D screen space.
   2. Fragment Shader: Processes every single pixel covered by those shapes to determine their exact final color.

## ⚙️ Why Your Project Specifically Needs It
Your architecture uses CUDA-OpenGL Interoperability for high performance. This pipeline works in a 3-step loop:

   1. CUDA Kernel: Calculates the complex Mandelbrot iteration math for every pixel and writes the results into a shared Pixel Buffer Object (PBO).
   2. Texture Mapping: The PBO transfers that raw mathematical array into an OpenGL Texture (essentially a digital canvas asset).
   3. The Shader Wrapper (shader.cpp): The vertex shader stretches a flat rectangle (two triangles) perfectly across your entire window. The fragment shader then samples your OpenGL texture and paints it onto that rectangle so you can see it.

Without this Shader wrapper, your CUDA-calculated data would sit trapped in graphics memory with no way to draw itself to the monitor.

## 📝 The Two Essential Shader Files
To make your Shader class work without throwing runtime errors, you need to create two text files inside your OpenGL/shaders/ directory matching the paths in your Renderer constructor.
## 1. fullscreen.vert (Vertex Shader)
This maps the geometric vertices defined in your createFullscreenQuad() method onto your monitor viewport.

## 2. fullscreen.frag (Fragment Shader)
This takes the texture generated from your PBO data and paints the calculated pixels onto the screen rectangle.



