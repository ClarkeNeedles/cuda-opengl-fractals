---
title: "Mandelbrot Function Research"
date: 6-17-2026
status: planning
version: 0.0.0
---

## Project Specifications: CUDA Mandelbrot Renderer (Float Precision Only)
The first version of this project strictly uses single-precision floating-point numbers (float) to maximize performance on the GPU.
## Architectural Overview

* Memory Mapping: An OpenGL Pixel Buffer Object (PBO) maps directly into the CUDA address space.
* Thread Mapping: Each individual CUDA thread calculates exactly one pixel on the screen. 

------------------------------
## The Mandelbrot Algorithm
### Core Equation
The fractal is generated on a complex plane where the X-axis represents real numbers and the Y-axis represents imaginary numbers. The sequence starts at zero and iterates using:  
$$Z_{n+1} = Z_n^2 + C$$

* Z₀: Starts at 0
* C: A complex constant representing the specific pixel's location on the complex plane.

### Escape Condition
The sequence is guaranteed to escape to infinity if the magnitude of Z exceeds 2 (|Z| > 2).
To optimize performance, we avoid computationally expensive square roots. The escape condition is rearranged into the following single-precision formula:
$$Z_r^2 + Z_i^2 > 4.0f$$ 

* Outside the Set: The pixel is outside the set if the equation hits this threshold.
* Inside the Set: The pixel is inside the set if the loop reaches the max_iterations cutoff without escaping.

------------------------------
## Coloring Implementation
### 1. Smooth Coloring (Renormalization)
To eliminate color banding, use the renormalization formula. This formula accounts for how far past the escape radius the value blew up:
$$v = \text{iteration} - \frac{\ln(\ln(\vert{}Z\vert{}))}{\ln(2)}$$
### 2. Palette Lookup
Map the smoothed value v to a color palette using a cosine-based function:
$$\text{color}(v) = A + B \cdot \cos(2\pi \cdot (C \cdot v + D))$$ 

* A, B, C, D: 3-component vectors (float3) defining the bias, amplitude, frequency, and phase of the color waves.
