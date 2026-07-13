/******************************************************************************
 * @file    kernel.cu
 * @author  Clarke Needles
 * @brief   CUDA implementation of the Mandelbrot fractal renderer.
 ******************************************************************************/

#include "kernel.cuh"

#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <math.h>

/**
 * @brief Device function to compute smooth RGB color from a normalized value.
 * @details Uses a cosine palette calculation to create classic electric blue/purple gradients.
 */
__device__ uchar4 get_palette_color(float t) 
{
    if (t >= 1.0f)
    {
        return make_uchar4(0, 0, 0, 255); // Inside the set defaults to solid black
    }

    // Cosine palette transformation parameters
    float3 p1 = make_float3(0.5f, 0.5f, 0.5f);
    float3 p2 = make_float3(0.5f, 0.5f, 0.5f);
    float3 p3 = make_float3(1.0f, 0.7f, 0.4f);
    float3 p4 = make_float3(0.00f, 0.15f, 0.20f);

    unsigned char r = (unsigned char)(255.0f * (p1.x + p2.x * cosf(2.0f * 3.14159f * (p3.x * t + p4.x))));
    unsigned char g = (unsigned char)(255.0f * (p1.y + p2.y * cosf(2.0f * 3.14159f * (p3.y * t + p4.y))));
    unsigned char b = (unsigned char)(255.0f * (p1.z + p2.z * cosf(2.0f * 3.14159f * (p3.z * t + p4.z))));

    return make_uchar4(r, g, b, 255);
}

/**
 * @brief Mandelbrot CUDA Kernel for parallel pixel evaluation.
 */
__global__ void mandelbrot_kernel(uchar4* buffer, int width, int height, 
    float zoom, float move_x, float move_y, int max_iters) 
{
    // Map current thread to unique global pixel coordinates
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    // Out-of-bounds safety boundary check
    if (x >= width || y >= height) 
        return;

    // Map screen pixel space to complex plane coordinates (C = cr + i*ci)
    float cr = 1.5f * (x - width / 2.0f) / (0.5f * zoom * width) + move_x;
    float ci = (y - height / 2.0f) / (0.5f * zoom * height) + move_y;

    // Initialize Z_0 = 0
    float zr = 0.0f;
    float zi = 0.0f;

    int iter = 0;
    float zr2 = 0.0f;
    float zi2 = 0.0f;

    // Main Escape Time Loop
    while (zr2 + zi2 <= 4.0f && iter < max_iters) 
    {
        zi = 2.0f * zr * zi + ci;
        zr = zr2 - zi2 + cr;

        zr2 = zr * zr;
        zi2 = zi * zi;
        iter++;
    }

    // Map raw iteration count to a continuous smooth color spectrum
    float t = 1.0f;
    if (iter < max_iters) 
    {
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
void launch_mandelbrot_kernel(uchar4* d_buffer, int width, int height, 
    float zoom, float move_x, float move_y, int max_iters) 
{
    // 16x16 pixel blocks are highly optimized for modern GPU architectures
    dim3 blockSize(16, 16);
    dim3 gridSize((width + blockSize.x - 1) / blockSize.x, 
        (height + blockSize.y - 1) / blockSize.y);

    mandelbrot_kernel << <gridSize, blockSize >> > (
        d_buffer, width, height, zoom, move_x, move_y, max_iters);

    // Explicit synchronization to ensure execution completes before OpenGL redraws
    cudaDeviceSynchronize();
}