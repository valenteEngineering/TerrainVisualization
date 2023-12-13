//
// Created by alexv on 10/26/2022.
//

#include <string>
#include <iostream>
#include "cstring"
#include "RenderableObject.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

void RenderableObject::LoadData(const char *filename, int x, int y, float hightScaler) {
    int n;
    width = x;
    height = y;
    stbi_ldr_to_hdr_gamma(1.0f);
    unsigned char *data = stbi_load((const char *) filename, &width, &height, &n, 4);
    image = std::vector<float>(data, data + width * height * 4);
    stbi_image_free(data);
    std::cout << "load image start" << std::endl;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            size_t index = 4 * (j * width + i);
            points.push_back((GLfloat) i);
            points.push_back(image[index] * hightScaler);
            points.push_back((GLfloat) j);
        }
    }
    std::cout << "load image end" << std::endl;
}

void RenderableObject::GetPoints(std::vector<float> &outPoints) {
    outPoints.insert(outPoints.begin(), points.begin(), points.end());
    outPoints = points;
}

void RenderableObject::GetPixel(int x, int y, float *outPoint) {
    size_t index = 4 * (y * width + x);
    float data[4] = {(float) image[index], (float) image[index + 1], (float) image[index + 2],
                     (float) image[index + 3]};
    memcpy(outPoint, data, sizeof(float) * 4);
}

int RenderableObject::GetWidth() {
    return width;
}

int RenderableObject::GetHeight() {
    return height;
}

