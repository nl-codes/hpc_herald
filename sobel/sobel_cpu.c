/************************************************************************
 * PROGRAM DESCRIPTION:
 * This program applies Sobel edge detection to a PNG image using
 * sequential CPU processing.
 *
 * CORE OPERATIONS:
 * 1. PNG Decoding: Uses LodePNG to load image into 32-bit RGBA array.
 * 2. Grayscale Conversion: Converts RGBA input to single-channel intensity.
 * 3. Sobel Edge Detection: Applies Gx and Gy 3x3 kernels with zero-padding,
 *    computes gradient magnitude sqrt(Gx² + Gy²) for each pixel.
 * 4. Value Clamping: Ensures output values stay in valid 0–255 range.
 * 5. Output Filename Generation: Automatically generates output filename
 *    by appending "_sobel_edge_output" before the ".png" extension.
 *
 * COMPILE WITH:
 * gcc lodepng.c sobel_cpu.c -o sobel_cpu -lm
 * (Note: -lm is needed for sqrt() from <math.h>)
 *
 * USAGE:
 * ./sobel_cpu input.png                  # output.png will be auto-generated
 * ./sobel_cpu input.png custom_output.png  # optional explicit output
 ************************************************************************/

#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>      // for sqrt()

 /* clamp helper: keep value in 0..255 */
static unsigned char clamp255(int v) {
    if (v < 0)   return 0;
    if (v > 255) return 255;
    return (unsigned char)v;
}

/* Generate output filename by appending "_sobel_edge_output" before the extension */
static void generate_output_filename(char* dest, size_t dest_size, const char* input_filename) {
    const char* ext = strrchr(input_filename, '.');  // find last '.'
    if (ext && strcasecmp(ext, ".png") == 0) {
        // Found ".png" extension
        size_t base_len = ext - input_filename;  // length without extension
        snprintf(dest, dest_size, "%.*s_sobel_edge_output.png", (int)base_len, input_filename);
    } else {
        // No .png extension or invalid → just append
        snprintf(dest, dest_size, "%s_sobel_edge_output.png", input_filename);
    }
}

/* Sobel edge detection on CPU */
static void sobel_cpu(unsigned char* img, unsigned width, unsigned height) {
    /* Allocate temporary grayscale buffer */
    unsigned char* gray = (unsigned char*)malloc(width * height);
    if (!gray) {
        fprintf(stderr, "Failed to allocate grayscale buffer\n");
        return;
    }

    /* Step 1: Convert RGBA to grayscale (luminance) */
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            size_t i = 4 * (y * width + x);
            unsigned char r = img[i + 0];
            unsigned char g = img[i + 1];
            unsigned char b = img[i + 2];
            int lum = (int)(0.299f * r + 0.587f * g + 0.114f * b);
            gray[y * width + x] = clamp255(lum);
        }
    }

    /* Allocate output buffer */
    unsigned char* output = (unsigned char*)malloc(width * height);
    if (!output) {
        fprintf(stderr, "Failed to allocate output buffer\n");
        free(gray);
        return;
    }

    /* Step 2: Apply Sobel operator with zero-padding */
    int Gx[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
    int Gy[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };

    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            int gx = 0, gy = 0;

            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int nx = (int)x + kx;
                    int ny = (int)y + ky;

                    unsigned char pixel = 0;
                    if (nx >= 0 && nx < (int)width && ny >= 0 && ny < (int)height) {
                        pixel = gray[ny * width + nx];
                    }

                    int kidx = (ky + 1) * 3 + (kx + 1);
                    gx += pixel * Gx[kidx];
                    gy += pixel * Gy[kidx];
                }
            }

            float mag = sqrtf((float)(gx * gx + gy * gy));
            output[y * width + x] = clamp255((int)(mag + 0.5f));
        }
    }

    /* Step 3: Convert single-channel edge map back to RGBA */
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            size_t i = 4 * (y * width + x);
            unsigned char edge = output[y * width + x];
            img[i + 0] = edge;
            img[i + 1] = edge;
            img[i + 2] = edge;
            img[i + 3] = 255;
        }
    }

    free(gray);
    free(output);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s input.png [output.png]\n", argv[0]);
        fprintf(stderr, "  If output.png is omitted, it will be auto-generated as input_sobel_edge_output.png\n");
        return 1;
    }

    const char* input = argv[1];
    char output_buffer[512];
    const char* output;

    if (argc >= 3) {
        output = argv[2];  // user provided explicit output filename
    } else {
        generate_output_filename(output_buffer, sizeof(output_buffer), input);
        output = output_buffer;
    }

    unsigned char* image = NULL;
    unsigned width, height;
    unsigned error;

    /* Decode PNG to 32-bit RGBA */
    error = lodepng_decode32_file(&image, &width, &height, input);
    if (error) {
        fprintf(stderr, "Decode error %u: %s\n",
            error, lodepng_error_text(error));
        return 1;
    }

    printf("Image: %s (%u x %u)\n", input, width, height);
    printf("Saving result to: %s\n", output);

    /* Apply Sobel edge detection on CPU */
    sobel_cpu(image, width, height);

    /* Encode result back to PNG */
    error = lodepng_encode32_file(output, image, width, height);
    if (error) {
        fprintf(stderr, "Encode error %u: %s\n",
            error, lodepng_error_text(error));
    } else {
        printf("Saved edge-detected image to %s\n", output);
    }

    free(image);
    return error ? 1 : 0;
}
