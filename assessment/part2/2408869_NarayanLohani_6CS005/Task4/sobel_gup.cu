/************************************************************************
 * PROGRAM DESCRIPTION:
 * This program applies Sobel edge detection to a PNG image using CUDA GPU.
 * Optimized version: Grayscale conversion is performed inside the kernel.
 *
 * CORE OPERATIONS:
 * 1. PNG Decoding: Loads 32-bit RGBA image using LodePNG.
 * 2. Data Transfer: Transfers full RGBA image to GPU.
 * 3. Full GPU Processing: Both grayscale conversion and Sobel convolution
 *    (with zero-padding and magnitude) done in one kernel.
 * 4. Result: Single-channel edge map returned and saved as grayscale PNG.
 * 5. Output Filename: Auto-generated or user-specified.
 *
 * COMPILE WITH:
 * nvcc lodepng.c sobel_gpu.cu -o sobel_gpu -lm
 *
 * USAGE:
 * ./sobel_gpu input.png                  # auto output: input_sobel_edge_output.png
 * ./sobel_gpu input.png custom.png       # explicit output
 ************************************************************************/

#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cuda_runtime.h>

 /* CUDA error checking macro */
#define CUDA_CHECK(err) do { if (err != cudaSuccess) { fprintf(stderr, "CUDA Error: %s at %s:%d\n", cudaGetErrorString(err), __FILE__, __LINE__); exit(1); } } while (0)

/* Device clamp function */
__device__ static unsigned char clamp255(int v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return (unsigned char)v;
}

/* Generate output filename */
static void generate_output_filename(char* dest, size_t dest_size, const char* input_filename) {
    const char* ext = strrchr(input_filename, '.');
    if (ext && strcasecmp(ext, ".png") == 0) {
        size_t base_len = ext - input_filename;
        snprintf(dest, dest_size, "%.*s_sobel_edge_output.png", (int)base_len, input_filename);
    } else {
        snprintf(dest, dest_size, "%s_sobel_edge_output.png", input_filename);
    }
}

/* Constant memory for Sobel kernels */
__constant__ int d_Gx[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
__constant__ int d_Gy[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };

/* CUDA Kernel: Full Sobel edge detection with on-the-fly grayscale conversion from RGBA */
__global__ void sobel_kernel(const unsigned char* rgba_input,
    unsigned char* output,
    int width, int height) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= width || y >= height) return;

    int gx = 0, gy = 0;

    // 3x3 neighborhood convolution
    for (int ky = -1; ky <= 1; ++ky) {
        for (int kx = -1; kx <= 1; ++kx) {
            int nx = x + kx;
            int ny = y + ky;

            unsigned char pixel = 0;  // zero-padding

            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                int idx = (ny * width + nx) * 4;  // RGBA index
                unsigned char r = rgba_input[idx + 0];
                unsigned char g = rgba_input[idx + 1];
                unsigned char b = rgba_input[idx + 2];

                // Convert to grayscale using luminance formula
                float gray_float = 0.299f * r + 0.587f * g + 0.114f * b;
                pixel = clamp255((int)(gray_float + 0.5f));  // round to nearest
            }

            int kidx = (ky + 1) * 3 + (kx + 1);
            gx += pixel * d_Gx[kidx];
            gy += pixel * d_Gy[kidx];
        }
    }

    float mag = sqrtf((float)(gx * gx + gy * gy));
    output[y * width + x] = clamp255((int)(mag + 0.5f));
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s input.png [output.png]\n", argv[0]);
        fprintf(stderr, "  If output.png is omitted, it will be auto-generated as <input>_sobel_edge_output.png\n");
        return 1;
    }

    const char* input_filename = argv[1];
    char output_buffer[512];
    const char* output_filename;

    if (argc >= 3) {
        output_filename = argv[2];
    } else {
        generate_output_filename(output_buffer, sizeof(output_buffer), input_filename);
        output_filename = output_buffer;
    }

    unsigned char* h_image = NULL;
    unsigned width, height;
    unsigned error;

    /* Decode PNG to 32-bit RGBA */
    error = lodepng_decode32_file(&h_image, &width, &height, input_filename);
    if (error) {
        fprintf(stderr, "Decode error %u: %s\n", error, lodepng_error_text(error));
        return 1;
    }

    /* Display image information */
    printf("Input Image      : %s\n", input_filename);
    printf("Image Dimensions : %u x %u pixels\n", width, height);
    printf("Output Image     : %s\n", output_filename);

    size_t pixels = width * height;

    /* Allocate host output buffer only (no need for grayscale buffer) */
    unsigned char* h_output = (unsigned char*)malloc(pixels);
    if (!h_output) {
        fprintf(stderr, "Host memory allocation failed\n");
        free(h_image);
        return 1;
    }

    /* Device memory */
    unsigned char* d_rgba, * d_output;
    CUDA_CHECK(cudaMalloc(&d_rgba, pixels * 4 * sizeof(unsigned char)));  // RGBA input
    CUDA_CHECK(cudaMalloc(&d_output, pixels * sizeof(unsigned char)));     // 1-channel output

    /* Copy full RGBA image to device */
    CUDA_CHECK(cudaMemcpy(d_rgba, h_image, pixels * 4, cudaMemcpyHostToDevice));

    /* 2D Launch configuration: 16x16 threads per block */
    dim3 blockDim(16, 16);
    dim3 gridDim(
        (width + blockDim.x - 1) / blockDim.x,
        (height + blockDim.y - 1) / blockDim.y
    );

    /* Launch kernel (grayscale + Sobel all on GPU) */
    sobel_kernel << <gridDim, blockDim >> > (d_rgba, d_output, width, height);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    /* Copy edge map back to host */
    CUDA_CHECK(cudaMemcpy(h_output, d_output, pixels, cudaMemcpyDeviceToHost));

    /* Convert single-channel edge map back to RGBA for PNG output */
    for (size_t i = 0; i < pixels; ++i) {
        unsigned char edge = h_output[i];
        h_image[4 * i + 0] = edge;
        h_image[4 * i + 1] = edge;
        h_image[4 * i + 2] = edge;
        h_image[4 * i + 3] = 255;  // opaque
    }

    /* Save output PNG */
    error = lodepng_encode32_file(output_filename, h_image, width, height);
    if (error) {
        fprintf(stderr, "Encode error %u: %s\n", error, lodepng_error_text(error));
    } else {
        printf("Sobel edge detection completed successfully.\n");
        printf("Edge map saved as: %s\n", output_filename);
    }

    /* Cleanup */
    cudaFree(d_rgba);
    cudaFree(d_output);
    free(h_output);
    free(h_image);

    return error ? 1 : 0;
}
