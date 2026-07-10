\---

title: "Project Start \& Architecture Refactor"

date: 2026-07-10

status: in-progress

version: 0.2.0

\---



\# Project Milestone: Core Architecture Setup



Fantastic progress! The foundational engineering proof-of-concept is complete. The build chain issues are resolved, and the system is successfully processing shared graphics memory.



\## 🚀 Accomplishments



\*   \*\*Fixed Build Chain Integration\*\*: Resolved the custom CUDA template driver and header linking conflicts.

\*   \*\*Interoperability Functional\*\*: Successfully established active memory buffer sharing between CUDA and OpenGL.

\*   \*\*RTX 3070 Processing\*\*: The GPU is actively computing iteration math on the device and drawing the Mandelbrot set directly onto the screen viewport canvas.



\---



\## 🏗️ New Target System Architecture



To prepare for complex features like dynamic input handling and high-fidelity timing metrics, the single-file layout will be refactored into a scalable, object-oriented module hierarchy managed via CMake.



```text

CUDA-Mandelbrot/

│

├── src/

│   ├── main.cpp                 <- Engine initialization entry point

│   ├── App.cpp                  <- Core application engine loops

│   ├── App.h

│   │

│   ├── OpenGL/

│   │   ├── OpenGLContext.cpp    <- Window management, GLFW/GLEW setup

│   │   ├── OpenGLContext.h

│   │   ├── Shader.cpp           <- Custom shader compilation utilities

│   │   └── Shader.h

│   │

│   ├── CUDA/

│   │   ├── mandelbrot\_kernel.cu <- Pure GPU device math algorithms

│   │   └── mandelbrot\_kernel.cuh

│   │

│   └── Renderer/

│       ├── MandelbrotRenderer.cpp <- Interop buffer locks and draw loops

│       └── MandelbrotRenderer.h

│

└── CMakeLists.txt               <- Cross-platform build script compilation rules

```



\---



\## 📋 Next Development Phases



\### 1. Code Refactoring (Current Objective)

\- \[ ] Migrate raw global variables into distinct context classes (`App`, `OpenGLContext`).

\- \[ ] Isolate pure CUDA calculation logic away from window management files.

\- \[ ] Construct the master `CMakeLists.txt` file to stitch these nested folders back together.



\### 2. User Input Integration

\- \[ ] Implement GLFW key callbacks for camera transformations (WASD / Arrows to pan).

\- \[ ] Implement mouse wheel hook structures to calculate smooth real-time scale zooming.



\### 3. Performance Benchmarking

\- \[ ] Integrate highly accurate hardware-level system clocks (using `std::chrono` or native CUDA Events).

\- \[ ] Calculate, track, and display real-time Frame Generation Times (ms) and FPS directly inside the window title bar.



