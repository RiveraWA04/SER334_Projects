/**
 * Author: William A Rivera Rosales
 * Version: 1/21/2024
 */

#include "BMPHandler-1.h"

// Read BMP header from a file
int readBMPHeader(FILE* file, struct BMP_Header* header) {
    return fread(header, sizeof(struct BMP_Header), 1, file) == 1;
}

// Write BMP header to a file
void writeBMPHeader(FILE* file, struct BMP_Header* header) {
    fwrite(header, sizeof(struct BMP_Header), 1, file);
}

// Read DIB header from a file
int readDIBHeader(FILE* file, struct DIB_Header* header) {
    return fread(header, sizeof(struct DIB_Header), 1, file) == 1;
}

// Write DIB header to a file
void writeDIBHeader(FILE* file, struct DIB_Header* header) {
    fwrite(header, sizeof(struct DIB_Header), 1, file);
}

void makeBMPHeader(struct BMP_Header* header, int width, int height) {
    const int bytesPerPixel = 3;
    int rowSize = (width * bytesPerPixel + 3) & (~3);
    int pixelArraySize = rowSize * height;

    header->signature[0] = 'B';
    header->signature[1] = 'M';
    header->size = 54 + pixelArraySize;
    header->reserved1 = 0;
    header->reserved2 = 0;
    header->offset_pixel_array = 54;
    printf("BMP Header:\n");
    printf("File Size: %d\n", header->size);
    printf("Pixel Array Offset: %d\n", header->offset_pixel_array);
}

void makeDIBHeader(struct DIB_Header* header, int width, int height) {
    header->header_size = 40;
    header->width = width;
    header->height = height;
    header->planes = 1;
    header->bit_per_pixel = 24;
    header->compression = 0;
    header->image_size = 0;
    header->x_pixels_per_meter = 2835;
    header->y_pixels_per_meter = 2835;
    header->colors_used = 0;
    header->colors_important = 0;
    printf("DIB Header:\n");
    printf("Width: %d, Height: %d\n", header->width, header->height);
}

// Read pixel data from BMP file
int readPixelsBMP(FILE* file, struct Pixel** pArr, int width, int height) {
    for (int i = 0; i < height; i++) {
        if (fread(pArr[i], sizeof(struct Pixel), width, file) != width) {
            return 0; // Failed to read pixel data
        }
        // Handle padding bytes at the end of each row
        fseek(file, (4 - (width * sizeof(struct Pixel)) % 4) % 4, SEEK_CUR);
    }
    return 1; // Successfully read pixel data
}

void writePixelsBMP(FILE* file, struct Pixel** pArr, int width, int height) {
    int padding = (4 - (width * sizeof(struct Pixel)) % 4) % 4;
    char pad[3] = {0};
    int totalBytesWritten = 0;

    for (int i = 0; i < height; i++) {
        int bytesWritten = fwrite(pArr[i], sizeof(struct Pixel), width, file);
        totalBytesWritten += bytesWritten;
        fwrite(pad, padding, 1, file);
        totalBytesWritten += padding;
    }

    printf("Total bytes written for pixel data: %d\n", totalBytesWritten);
}

