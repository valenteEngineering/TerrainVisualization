//
// Created by alexv on 10/26/2022.
//

#ifndef UNTITLED_RENDERABLEOBJECT_H
#define UNTITLED_RENDERABLEOBJECT_H
#include <GL/glu.h>
#include <vector>

class RenderableObject {
public:
    void LoadData(const char *filename, int width, int height, float hightScaler);
    void GetPixel(int x, int y, float* outPoint);
    int GetWidth();
    int GetHeight();
    void GetPoints(std::vector<float> &outPoints);

private:
    std::vector<GLfloat> points;
    std::vector<float> image;
    int width;
    int height;
};


#endif //UNTITLED_RENDERABLEOBJECT_H
