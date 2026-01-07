#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Clamp helper */
static unsigned char clamp255(int v) {
    if (v < 0)   return 0;
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
        snprintf(dest, dest_size, "%s__gray_output.png", input_filename);
    }
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

    /* Decode PNG */
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

    /* Allocate host buffers */
    unsigned char* h_gray = (unsigned char*)malloc(pixels);
    unsigned char* h_output = (unsigned char*)malloc(pixels);
    if (!h_gray || !h_output) {
        fprintf(stderr, "Host memory allocation failed\n");
        free(h_image);
        return 1;
    }

    /* Convert RGBA → Grayscale on CPU */
    for (size_t i = 0; i < pixels; ++i) {
        unsigned char r = h_image[4 * i + 0];
        unsigned char g = h_image[4 * i + 1];
        unsigned char b = h_image[4 * i + 2];
        int lum = (int)(0.299f * r + 0.587f * g + 0.114f * b);
        h_gray[i] = clamp255(lum);
    }


    /* Convert grayscale back to RGBA */
    for (size_t i = 0; i < pixels; ++i) {
        unsigned char gray_value = h_gray[i];
        h_image[4 * i + 0] = gray_value;
        h_image[4 * i + 1] = gray_value;
        h_image[4 * i + 2] = gray_value;
        h_image[4 * i + 3] = 255;
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
    free(h_gray);
    free(h_output);
    free(h_image);

    return error ? 1 : 0;
}
