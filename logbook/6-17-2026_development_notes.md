a grid contains thousands of blocks
a single block can only be executed on one SM (cannot be split accross blocks), although if hardware allows, one SM can host multiple blocks
blocks are broken into hardware groups of 32 parallel threads (called a warp)
an SM executes one instruction for an entire warp at the same exact time
block size matters
	if blocksize is 16x16 = 256 threads, the hardware divides this block into 8 warps
	if block size is not a multiple of 32, you are wasting hardware capacity
	
blockDim (the block dimensions)
	the size of a single block
	how many threads are packed into a single block
blockIdx (block coordinate)
	position of the current block inside the entire overall grid
threadIdx (threads local coordinate)
	position of a thread inside its parent block
	for a 16x16 block, this ranges from (0,0) to (15,15)
	
to get the absolute pixel position on screen, the thread scales its block index by the block dimensions and adds its internal thread offset
int x = blockIdx.x * blockDim.x + threadIdx.x;
int y = blockIdx.y * blockDim.y + threadIdx.y;

mandelbrot set and warp divergence
a warp executes 32 threads doing the same thing at the exact same time
if one thread escapes the set in 10 iterations, it may be stuck waiting for a thread inside the set and running for the maximum iterations
the thread that finishes early is idle and waste while waiting

below is example code on how a mandelbrot kernel should look

#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <math.h>

// Device function to compute smooth RGB color from normalized value
__device__ uchar4 get_palette_color(float t) {
    if (t >= 1.0f) return make_uchar4(0, 0, 0, 255); // Inside the set is black

    // Cosine palette parameters (Classic electric blue/purple palette)
    float3 a = make_float3(0.5f, 0.5f, 0.5f);
    float3 b = make_float3(0.5f, 0.5f, 0.5f);
    float3 c = make_float3(1.0f, 0.7f, 0.4f);
    float3 d = make_float3(0.00f, 0.15f, 0.20f);

    unsigned char r = (unsigned char)(255.0f * (a.x + b.x * cosf(2.0f * 3.14159f * (c.x * t + d.x))));
    unsigned char g = (unsigned char)(255.0f * (a.y + b.y * cosf(2.0f * 3.14159f * (c.y * t + d.y))));
    unsigned char b_val = (unsigned char)(255.0f * (a.z + b.z * cosf(2.0f * 3.14159f * (c.z * t + d.z))));

    return make_uchar4(r, g, b_val, 255);
}

// Mandelbrot CUDA Kernel
__global__ void mandelbrot_kernel(uchar4* buffer, int width, int height, float zoom, float moveX, float moveY, int max_iters) {
    // Map thread to pixel coordinates
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    // Boundary check
    if (x >= width || y >= height) return;

    // Map screen pixel to complex plane (C = cr + i*ci)
    // Adjusts for aspect ratio and applies zoom/pan offsets
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

    // Map iteration count to smooth color spectrum
    float t = 1.0f;
    if (iter < max_iters) {
        // Logarithmic smoothing formula to eradicate banding
        float log_zn = logf(zr2 + zi2) / 2.0f;
        float nu = logf(log_zn / logf(2.0f)) / logf(2.0f);
        t = ((float)iter + 1.0f - nu) / (float)max_iters;
    }

    // Write RGBA value straight to the shared OpenGL PBO pointer
    int pixel_index = y * width + x;
    buffer[pixel_index] = get_palette_color(t);
}

// Host wrapper to handle execution configuration
extern "C" void launch_mandelbrot(uchar4* d_buffer, int width, int height, float zoom, float moveX, float moveY, int max_iters) {
    // 16x16 or 32x32 pixel blocks are highly optimized for modern GPU architectures
    dim3 blockSize(16, 16);
    dim3 gridSize((width + blockSize.x - 1) / blockSize.x, (height + blockSize.y - 1) / blockSize.y);

    mandelbrot_kernel<<<gridSize, blockSize>>>(d_buffer, width, height, zoom, moveX, moveY, max_iters);
    
    // Synch to ensure execution completes before OpenGL redraws
    cudaDeviceSynchronize();
}