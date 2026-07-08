---
title: "CUDA Kernel Structure"
date: 6-18-2026
status: planning
version: 0.0.0
---

# CUDA Mandelbrot Architecture and Implementation Guide
## 1. CUDA Hardware Architecture Foundations
### Block Execution and Streaming Multiprocessors (SM)

* SM Assignment: A single block executes entirely on one SM and cannot be split across multiple SMs.
* SM Multitenancy: One SM can host multiple concurrent blocks if hardware resources (registers, shared memory) allow.
* Grid Scaling: A standard execution grid contains thousands of individual blocks to fully saturate the GPU.

### Warps and Execution Mechanics

* Warp Size: Blocks are automatically subdivided by the hardware into groups of 32 parallel threads called a warp.
* SIMT Execution: An SM executes exactly one instruction for an entire warp at the exact same physical time.
* Capacity Optimization: If a block size is not a multiple of 32, hardware capacity is permanently wasted during execution.
* Optimized Configurations: Block dimensions of 16 × 16 (256 threads) divide cleanly into exactly 8 warps.

### Coordinate Mapping Space

* blockDim: The physical size and dimensions of a single thread block.
* blockIdx: The coordinates and position of the current block inside the global execution grid.
* threadIdx: The internal, local coordinate of a thread inside its immediate parent block.
* Index Range: For a 16 × 16 block configuration, threadIdx ranges from (0,0) to (15,15).

------------------------------
## 2. Mathematical Mapping and Warp Divergence
### Absolute Pixel Calculation
To determine the absolute target pixel on a screen display, a thread scales its block index by the block dimensions and adds its internal thread offset:
$$\text{Position}_x = \text{blockIdx.x} \times \text{blockDim.x} + \text{threadIdx.x}$$ 
$$\text{Position}_y = \text{blockIdx.y} \times \text{blockDim.y} + \text{threadIdx.y}$$ 
### The Mandelbrot Divergence Penalty
Warp divergence occurs when threads inside the same 32-thread warp execute different instruction paths.

Warp Thread Pipeline:
[T1: Iter 10 (Idle)] ---> Waiting for T2...
[T2: Iter 1000 (Active)] -----------------------------------------> Done


* Lock-step Execution: Because a warp executes the same instruction simultaneously, all 32 threads must wait for the longest-running thread.
* Escape Time Variance: If one thread escapes the Mandelbrot set in 10 iterations, it sits idle while a neighboring thread inside the set runs to max_iters.
* Hardware Waste: Early-terminating threads create immediate hardware waste while waiting for late-terminating threads to finish the loop.

------------------------------
## 3. Production Implementation
Below is the complete CUDA source code utilizing an optimized layout, aspect-ratio correction, logarithmic smoothing, and a cosine color palette.

```
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <math.h>

/**
 * @brief Device function to compute smooth RGB color from a normalized value.
 * @details Uses a cosine palette calculation to create classic electric blue/purple gradients.
 */
__device__ uchar4 get_palette_color(float t) {
    if (t >= 1.0f) return make_uchar4(0, 0, 0, 255); // Inside the set defaults to solid black

    // Cosine palette transformation parameters
    float3 a = make_float3(0.5f, 0.5f, 0.5f);
    float3 b = make_float3(0.5f, 0.5f, 0.5f);
    float3 c = make_float3(1.0f, 0.7f, 0.4f);
    float3 d = make_float3(0.00f, 0.15f, 0.20f);

    unsigned char r = (unsigned char)(255.0f * (a.x + b.x * cosf(2.0f * 3.14159f * (c.x * t + d.x))));
    unsigned char g = (unsigned char)(255.0f * (a.y + b.y * cosf(2.0f * 3.14159f * (c.y * t + d.y))));
    unsigned char b_val = (unsigned char)(255.0f * (a.z + b.z * cosf(2.0f * 3.14159f * (c.z * t + d.z))));

    return make_uchar4(r, g, b_val, 255);
}

/**
 * @brief Mandelbrot CUDA Kernel for parallel pixel evaluation.
 */
__global__ void mandelbrot_kernel(uchar4* buffer, int width, int height, float zoom, float moveX, float moveY, int max_iters) {
    // Map current thread to unique global pixel coordinates
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    // Out-of-bounds safety boundary check
    if (x >= width || y >= height) return;

    // Map screen pixel space to complex plane coordinates (C = cr + i*ci)
    float cr = 1.5f * (x - width / 2.0f) / (0.5f * zoom * width) + moveX;
    float ci = (y - height / 2.0f) / (0.5f * zoom * height) + moveY;

    // Initialize Z_0 = 0
    float zr = 0.0f;
    float zi = 0.0f;
    
    int iter = 0;
    float zr2 = 0.0f;
    float zi2 = 0.0f;

    // Main Escape Time Loop
    while (zr2 + zi2 <= 4.0f && iter < max_iters) {
        zi = 2.0f * zr * zi + ci;
        zr = zr2 - zi2 + cr;
        
        zr2 = zr * zr;
        zi2 = zi * zi;
        iter++;
    }

    // Map raw iteration count to a continuous smooth color spectrum
    float t = 1.0f;
    if (iter < max_iters) {
        // Logarithmic smoothing formula to entirely eradicate color banding
        float log_zn = logf(zr2 + zi2) / 2.0f;
        float nu = logf(log_zn / logf(2.0f)) / logf(2.0f);
        t = ((float)iter + 1.0f - nu) / (float)max_iters;
    }

    // Write final RGBA value straight to the shared OpenGL PBO pointer allocation
    int pixel_index = y * width + x;
    buffer[pixel_index] = get_palette_color(t);
}

/**
 * @brief Host wrapper to configure execution parameters and launch the kernel.
 */
extern "C" void launch_mandelbrot(uchar4* d_buffer, int width, int height, float zoom, float moveX, float moveY, int max_iters) {
    // 16x16 pixel blocks are highly optimized for modern GPU architectures
    dim3 blockSize(16, 16);
    dim3 gridSize((width + blockSize.x - 1) / blockSize.x, (height + blockSize.y - 1) / blockSize.y);

    mandelbrot_kernel<<<gridSize, blockSize>>>(d_buffer, width, height, zoom, moveX, moveY, max_iters);
    
    // Explicit synchronization to ensure execution completes before OpenGL redraws
    cudaDeviceSynchronize();
}
```
