/******************************************************************************
 * @file    kernel.cuh
 * @author  Clarke Needles
 * @brief   CUDA header file for the Mandelbrot fractal renderer.
 ******************************************************************************/

#pragma once

constexpr int MAX_ITERS = 1000;

extern "C" void launch_mandelbrot_kernel(
    uchar4* d_buffer,
    int width,
    int height,
    float zoom,
    float moveX,
    float moveY
);