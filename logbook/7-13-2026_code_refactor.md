---
title: "Code refactor"
date: 7-13-2026
status: in-progress
version: 0.2.1
---

## 🛠️ Current Refactor Goals
*   **Modularity**: Begin restructuring the codebase to isolate fractal components.
*   **Extensibility**: Ensure the new architecture allows easy addition of new fractal types.
*   **Documentation**: Add comprehensive code comments across all modified source files.
*   **Build Stability**: Fix compilation breaks. Continue refactoring until a working build is achieved.

---

## 💡 Architecture & Design Ideas

### Configuration Management (`config.h`)
Create a centralized `config.h` file to store global constants. This avoids hardcoded values and unifies application parameters.
*   `MAX_ITERATIONS`
*   `SCREEN_WIDTH`
*   `SCREEN_HEIGHT`

**Implementation Plan:** Include `config.h` inside `main.cpp`. Pass the configuration object directly into the `App` constructor to let it manage internal initialization.

### Initialization & Lifecycle Dependencies
*   Move the window initialization pipeline directly into the `m_renderer` constructor.
*   **Dependency Warning**: The OpenGL context window (`m_window`) **must** be fully initialized before creating `m_renderer`. 
*   Always ensure the context is active and function pointers are loaded before executing OpenGL specific calls.

---

## 📊 Performance Benchmarks
CPU generation times gathered from the sister repository [ClarkeNeedles/FractalGenerator](https://github.com/ClarkeNeedles/FractalGenerator).

### Test Environment
*   **Resolution**: 900x600 window
*   **Hardware**: 11th Gen Intel(R) Core(TM) i7-1165G7 @ 2.80GHz
*   **Topology**: 2803 Mhz, 4 Physical Cores, 8 Logical Processors
*   **Sample Size**: 5 runs per configuration

### Benchmark Results

| Architecture / Optimization | Execution Time Range (ms) |
| :--- | :--- |
| C++ (Standard) | 1766.80 - 1854.21 ms |
| C++ & Multithreading | 767.21 - 804.78 ms |
| SSE (SIMD 128-bit) | 461.98 - 503.35 ms |
| SSE & Multithreading | 196.09 - 214.47 ms |
| AVX (SIMD 256-bit) | 239.81 - 259.39 ms |
| **AVX & Multithreading** | **107.37 - 111.56 ms** |

---

## 🎨 The Purpose of Shaders in Modern OpenGL
Modern OpenGL uses a strict programmable pipeline. The graphics card does not inherently know how to paint shapes or display raw pixel arrays. You must supply two specialized shaders to get anything onto the screen:

1.  **Vertex Shader**: Calculates where shapes sit in 2D or 3D screen space.
2.  **Fragment Shader**: Processes every single pixel covered by those shapes to determine their exact final color.

### ⚙️ Why Your Project Specifically Needs It
Your architecture uses CUDA-OpenGL Interoperability for high performance. This pipeline works in a 3-step loop:

1.  **CUDA Kernel**: Calculates the complex Mandelbrot iteration math for every pixel and writes the results into a shared Pixel Buffer Object (PBO).
2.  **Texture Mapping**: The PBO transfers that raw mathematical array into an OpenGL Texture (essentially a digital canvas asset).
3.  **The Shader Wrapper (`shader.cpp`)**: The vertex shader stretches a flat rectangle (two triangles) perfectly across your entire window. The fragment shader then samples your OpenGL texture and paints it onto that rectangle so you can see it.

Without this Shader wrapper, your CUDA-calculated data would sit trapped in graphics memory with no way to draw itself to the monitor.

---

## 📝 The Two Essential Shader Files
To make your `Shader` class work without throwing runtime errors, you need to create two text files inside your `OpenGL/shaders/` directory matching the paths in your Renderer constructor.

### 1. `fullscreen.vert` (Vertex Shader)
This maps the geometric vertices defined in your `createFullscreenQuad()` method onto your monitor viewport.

### 2. `fullscreen.frag` (Fragment Shader)
This takes the texture generated from your PBO data and paints the calculated pixels onto the screen rectangle.
