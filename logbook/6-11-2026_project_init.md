---
title: "Project Plan: Standalone CUDA-OpenGL Mandelbrot Benchmark Suite"
date: 06-11-2026
status: in-progress
version: 0.0.0
---

# Log Notes
Setting up a brand new standalone project repository to build a high-performance Mandelbrot fractal renderer using CUDA and OpenGL. This app will act as a standalone companion to my existing WinAPI CPU/SIMD fractal generator so I can document and compare the benchmark performance differences between specialized CPU vector units and raw GPU cores.

### Project Goals & Architecture
- Maintain complete separation from the WinAPI project to avoid messy configuration dependencies.
- Implement a continuous frame loop running at 60+ FPS that completely redraws the complex plane on the GPU every frame during interaction.
- Maximize performance using CUDA-OpenGL Interoperability (Pixel Buffer Objects) to write pixel colors directly to the GPU's texture memory, bypassing the CPU.
- Implement a smooth zoom and drag-to-pan feature by mapping mouse event deltas to complex coordinate shifts.
- Build a unified template kernel supporting both `float` and `double` precision variables. Track the zoom factor dynamically and hot-swap to the `double` kernel once precision limits (`1e-7`) are reached.

### Phased Development Roadmap

#### Phase 1: Environment Setup & Windowing (Est. 2-3 Hours)
- Create a clean project using CMake or standard Visual Studio configurations with CUDA build customizations enabled.
- Link GLFW (or SDL2) and GLEW/GLAD to spin up a basic rendering context window.
- Implement the decoupled main `while` loop that runs continuously independent of input callbacks.

#### Phase 2: Core CUDA Math & Single Precision (Est. 4-6 Hours)
- Define the baseline Mandelbrot kernel math handling the standard iterative sequence: \(z_{n+1} = z_n^2 + c\).
- Map individual 2D thread IDs (\((x, y)\) grid) to physical pixel addresses and their corresponding coordinates on the complex plane.
- Implement basic uniform variables for `centerX`, `centerY`, and `zoom` to control rendering boundaries.

#### Phase 3: OpenGL Texture Interop via PBOs (Est. 3-4 Hours)
- Set up an OpenGL texture buffer and register it to CUDA via `cudaGraphicsGLRegisterBuffer`.
- Write the interop boilerplate mapping the pointer right before kernel launch and unmapping it immediately after execution finishes.
- Verify zero-copy performance where data stays strictly on the GPU memory bus.

#### Phase 4: Smooth Interactivity & Double Precision (Est. 5-7 Hours)
- Wire mouse drag and scroll events to modify CPU parameters. Apply scale adjustments so the fractal locks perfectly to the cursor position during pans.
- Duplicate or template the kernel to support `double` precision math blocks.
- Write the application logic to hot-swap between the fast `float` path and the deeper `double` path depending on the zoom depth.
- Account for warp divergence in project notes, identifying performance cliffs at the boundaries of the Mandelbrot set.
