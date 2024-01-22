/**
 * Author: William A Rivera Rosales
 * Version: 1/21/2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BMPHandler-1.h"
#include "Image.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file_name> [options]\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* inputFileName = argv[1];
    char* outputFileName = "output.bmp"; // Default output file name if not provided

    // Open the BMP file and read the headers
    FILE* inputFile = fopen(inputFileName, "rb");
    if (inputFile == NULL) {
        fprintf(stderr, "Error: Unable to open file %s\n", inputFileName);
        return EXIT_FAILURE;
    }

    struct BMP_Header bmpHeader;
    struct DIB_Header dibHeader;

    if (!readBMPHeader(inputFile, &bmpHeader)) {
        fprintf(stderr, "Error: Unable to read BMP header from file %s\n", inputFileName);
        fclose(inputFile);
        return EXIT_FAILURE;
    }

    if (!readDIBHeader(inputFile, &dibHeader)) {
        fprintf(stderr, "Error: Unable to read DIB header from file %s\n", inputFileName);
        fclose(inputFile);
        return EXIT_FAILURE;
    }

    // Allocate memory for the pixel array
    struct Pixel** pixels = (struct Pixel**)malloc(dibHeader.height * sizeof(struct Pixel*));
    if (pixels == NULL) {
        fclose(inputFile);
        fprintf(stderr, "Error: Unable to allocate memory for pixel data\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < dibHeader.height; i++) {
        pixels[i] = (struct Pixel*)malloc(dibHeader.width * sizeof(struct Pixel));
        if (pixels[i] == NULL) {
            // Free previously allocated rows
            for (int j = 0; j < i; j++) {
                free(pixels[j]);
            }
            free(pixels);
            fclose(inputFile);
            fprintf(stderr, "Error: Unable to allocate memory for pixel row %d\n", i);
            return EXIT_FAILURE;
        }
    }

    // Read the pixel data into the allocated array
    if (!readPixelsBMP(inputFile, pixels, dibHeader.width, dibHeader.height)) {
        fprintf(stderr, "Error: Unable to read pixel data from file %s\n", inputFileName);
        for (int i = 0; i < dibHeader.height; i++) {
            free(pixels[i]);
        }
        free(pixels);
        fclose(inputFile);
        return EXIT_FAILURE;
    }
    fclose(inputFile);

    // Create an Image
    Image* image = image_create(pixels, dibHeader.width, dibHeader.height);
    if (image == NULL) {
        fprintf(stderr, "Error: Unable to create image structure\n");
        for (int i = 0; i < dibHeader.height; i++) {
            free(pixels[i]);
        }
        free(pixels);
        return EXIT_FAILURE;
    }

    // Apply filters based on command line arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-w") == 0) {
            image_apply_bw(image);
        } else if (strcmp(argv[i], "-r") == 0 && (i + 1) < argc) {
            image_apply_colorshift(image, atoi(argv[++i]), 0, 0);
        } else if (strcmp(argv[i], "-g") == 0 && (i + 1) < argc) {
            image_apply_colorshift(image, 0, atoi(argv[++i]), 0);
        } else if (strcmp(argv[i], "-b") == 0 && (i + 1) < argc) {
            image_apply_colorshift(image, 0, 0, atoi(argv[++i]));
        } else if (strcmp(argv[i], "-s") == 0 && (i + 1) < argc) {
            image_apply_resize(image, atof(argv[++i]));
        } else if (strcmp(argv[i], "-o") == 0 && (i + 1) < argc) {
            outputFileName = argv[++i];
        }
    }

    // Write the processed image to file
    FILE* outputFile = fopen(outputFileName, "wb");
    if (!outputFile) {
        fprintf(stderr, "Error: Unable to open output file %s\n", outputFileName);
        image_destroy(&image);
        return EXIT_FAILURE;
    }

    writeBMPHeader(outputFile, &bmpHeader);
    writeDIBHeader(outputFile, &dibHeader);
    writePixelsBMP(outputFile, image_get_pixels(image), image_get_width(image), image_get_height(image));

    fclose(outputFile); // Make sure to close the file
    image_destroy(&image); // Free the image struct and pixel array

    return EXIT_SUCCESS;
}
