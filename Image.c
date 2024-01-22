/**
 * Author: William A Rivera Rosales
 * Version: 1/21/2024
 */

#include "Image.h"

Image* image_create(struct Pixel** pArr, int width, int height) {
    Image* img = (Image*)malloc(sizeof(Image));
    if (img != NULL) {
        img->pArr = pArr;
        img->width = width;
        img->height = height;
    }
    return img;
}

void image_destroy(Image** img) {
    if (img != NULL && *img != NULL) {
        for (int i = 0; i < (*img)->height; i++) {
            free((*img)->pArr[i]);
        }
        free((*img)->pArr);
        free(*img);
        *img = NULL;
    }
}

struct Pixel** image_get_pixels(Image* img) {
    return img->pArr;
}

int image_get_width(Image* img) {
    return img->width;
}

int image_get_height(Image* img) {
    return img->height;
}

void image_apply_bw(Image* img) {
    if (img == NULL || img->pArr == NULL) return;
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            unsigned char gray = img->pArr[i][j].red * 0.299 +
                                 img->pArr[i][j].green * 0.587 +
                                 img->pArr[i][j].blue * 0.114;
            img->pArr[i][j].red = gray;
            img->pArr[i][j].green = gray;
            img->pArr[i][j].blue = gray;
        }
    }
}

void image_apply_colorshift(Image* img, int rShift, int gShift, int bShift) {
    if (img == NULL || img->pArr == NULL) return;
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            int red   = (int)img->pArr[i][j].red + rShift;
            int green = (int)img->pArr[i][j].green + gShift;
            int blue  = (int)img->pArr[i][j].blue + bShift;

            img->pArr[i][j].red   = red < 0 ? 0 : (red > 255 ? 255 : red);
            img->pArr[i][j].green = green < 0 ? 0 : (green > 255 ? 255 : green);
            img->pArr[i][j].blue  = blue < 0 ? 0 : (blue > 255 ? 255 : blue);
        }
    }
}

void image_apply_resize(Image* img, float factor) {
    if (img == NULL || img->pArr == NULL || factor <= 0) return;

    int newWidth = (int)(img->width * factor);
    int newHeight = (int)(img->height * factor);
    struct Pixel** newPixels = (struct Pixel**)malloc(newHeight * sizeof(struct Pixel*));

    for (int i = 0; i < newHeight; i++) {
        newPixels[i] = (struct Pixel*)malloc(newWidth * sizeof(struct Pixel));
        for (int j = 0; j < newWidth; j++) {
            int oldX = (int)(j / factor);
            int oldY = (int)(i / factor);
            newPixels[i][j] = img->pArr[oldY][oldX];
        }
    }

    // Free old pixel data
    for (int i = 0; i < img->height; i++) {
        free(img->pArr[i]);
    }
    free(img->pArr);

    img->pArr = newPixels;
    img->width = newWidth;
    img->height = newHeight;
}
