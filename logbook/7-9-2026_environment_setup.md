---
title: "Environment Setup"
date: 2026-07-09
status: completed
version: 0.2.0
---

# CUDA & OpenGL Development Environment Setup

Follow these steps to prepare your system for GPU-accelerated computing and real-time graphics rendering.

## 📋 Prerequisites

Before installing any software, verify your hardware and driver compatibility:

* **NVIDIA GPU**: Confirm your system has an NVIDIA graphics card with CUDA capabilities (e.g., RTX 3070).
* **Graphics Drivers**: Ensure your installed NVIDIA driver supports the version of the CUDA Toolkit you intend to use.
* **Git**: Install Git for Windows if it is not already available. This is required to clone the `vcpkg` repository.

---

## 🛠️ Installation Steps

Install all required software before beginning development.

### 1. Install Visual Studio Community

* Download and run the Visual Studio Community installer.
* Select the **Desktop development with C++** workload.
* Complete the installation.

### 2. Install the NVIDIA CUDA Toolkit

* Download the CUDA Toolkit version compatible with your installed NVIDIA drivers.
* Run the installer and complete the installation using the default options unless your project requires a custom configuration.

### 3. Install vcpkg

Open **PowerShell** and clone the official Microsoft `vcpkg` repository:

```powershell
git clone https://github.com/microsoft/vcpkg.git
```

Navigate into the cloned repository:

```powershell
cd vcpkg
```

Bootstrap `vcpkg`:

```powershell
.\bootstrap-vcpkg.bat
```

This builds the `vcpkg.exe` executable.

### 4. Integrate vcpkg with Visual Studio

From inside the `vcpkg` directory, run:

```powershell
.\vcpkg integrate install
```

This enables Visual Studio to automatically locate libraries installed through `vcpkg`.

### 5. Install the OpenGL Libraries

From the same `vcpkg` directory, install the required packages for a 64-bit Windows project:

```powershell
.\vcpkg install glew:x64-windows glfw3:x64-windows
```

This installs:

* **GLEW** – OpenGL Extension Wrangler Library
* **GLFW** – Window creation, input handling, and OpenGL context management

After installation, restart Visual Studio so the integration is detected.

---

## 🚀 Verification

Create a new CUDA project in Visual Studio and verify that your environment is configured correctly.

### Required Headers

The following headers should resolve without IntelliSense or compiler errors:

```cpp
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cuda_gl_interop.h>
```

### Link OpenGL

Add the native Windows OpenGL library to your project:

**Project Properties → Linker → Input → Additional Dependencies**

```
opengl32.lib
```

### Build Test

Verify that:

* The project compiles successfully.
* Visual Studio resolves the GLEW and GLFW headers.
* CUDA code compiles correctly.
* OpenGL creates a rendering window.
* CUDA and OpenGL can share graphics resources (such as Pixel Buffer Objects) using `cuda_gl_interop`.

Successful completion of these steps confirms that your CUDA, OpenGL, GLFW, GLEW, and `vcpkg` development environment is ready for GPU-accelerated graphics programming.