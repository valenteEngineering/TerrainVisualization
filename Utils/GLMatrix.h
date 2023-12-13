//
// Created by alexv on 10/25/2022.
//

#ifndef UNTITLED_GLMATRIX_H
#define UNTITLED_GLMATRIX_H

#include <glm/glm.hpp>


class GLMatrix {
public:

    enum MatrixMode {ModelView, ProjectionView, NumMatrixModes};

    void LookAt(float eyeX, float eyeY, float eyeZ,
                float centerX, float centerY, float centerZ,
                float upX, float upY, float upZ);
    void Translate(float x, float y, float z);
    void Rotate(float theta, float x, float y, float z);
    void Scale(float x, float y, float z);

    void Perspective(float fov, float aspect, float zNear, float zFar);
    void SetMode(MatrixMode mode);
    void SetModelMatrix(float* modelMat);
    void LoadIdentity();

    void getCurrentMat(float* mode);

protected:
    MatrixMode currentMode;
    glm::mat4 currentMatrix[NumMatrixModes];

    void multiplyToCurr(const glm::mat4 &m);
};


#endif //UNTITLED_GLMATRIX_H
