#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lodepng.h"  // Include the LodePNG header (single-file library)

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <image1.png> <image2.png>\n", argv[0]);
        printf("This program checks if two PNG images are identical (pixel-by-pixel).\n");
        return 1;
    }

    const char* filename1 = argv[1];
    const char* filename2 = argv[2];

    unsigned char* image1 = NULL;
    unsigned char* image2 = NULL;
    unsigned width1, height1;
    unsigned width2, height2;

    // Decode first image to 32-bit RGBA
    unsigned error1 = lodepng_decode32_file(&image1, &width1, &height1, filename1);
    if (error1) {
        printf("Error decoding %s: %u - %s\n", filename1, error1, lodepng_error_text(error1));
        return 1;
    }

    // Decode second image to 32-bit RGBA
    unsigned error2 = lodepng_decode32_file(&image2, &width2, &height2, filename2);
    if (error2) {
        printf("Error decoding %s: %u - %s\n", filename2, error2, lodepng_error_text(error2));
        free(image1);
        return 1;
    }

    // Check dimensions first
    if (width1 != width2 || height1 != height2) {
        printf("Images are DIFFERENT (different dimensions: %ux%u vs %ux%u)\n",
            width1, height1, width2, height2);
        free(image1);
        free(image2);
        return 0;
    }

    // Compare pixel data (RGBA, 4 bytes per pixel)
    size_t pixel_count = (size_t)width1 * height1 * 4;
    if (memcmp(image1, image2, pixel_count) == 0) {
        printf("Images are IDENTICAL.\n");
    } else {
        printf("Images are DIFFERENT (pixel data mismatch).\n");
    }

    // Clean up
    free(image1);
    free(image2);

    return 0;
}
