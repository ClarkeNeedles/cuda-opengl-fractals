/******************************************************************************
 * @file    kernel.cuh
 * @author  Clarke Needles
 * @brief   CUDA header file for the Mandelbrot fractal renderer.
 ******************************************************************************/

#pragma once

extern "C" void launch_mandelbrot_kernel(
    uchar4* d_buffer,
    int width,
    int height,
    float zoom,
    float move_x,
    float move_y,
    int max_iters
);