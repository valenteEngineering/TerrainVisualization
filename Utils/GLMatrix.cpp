//
// Created by alexv on 10/25/2022.
//

#include "cstring"

#include "GLMatrix.h"

#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

void GLMatrix::multiplyToCurr(const glm::mat4 &m) {
    currentMatrix[currentMode] = currentMatrix[currentMode] * m;
}

void GLMatrix::SetMode(GLMatrix::MatrixMode mode) {
    this->currentMode = mode;
}

void GLMatrix::LookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX,
                      float upY, float upZ) {
    glm::mat4 viewMat = glm::lookAt(glm::vec3(eyeX, eyeY, eyeZ),
                                    glm::vec3(centerX, centerY, centerZ),
                                    glm::vec3(upX, upY, upZ));
    multiplyToCurr(viewMat);
}

void GLMatrix::Rotate(float theta, float x, float y, float z) {
    glm::mat4 rotMat = glm::rotate(glm::radians(theta), glm::vec3(x, y, z));
    multiplyToCurr(rotMat);
}

void GLMatrix::Translate(float x, float y, float z) {
    currentMatrix[currentMode] = glm::translate(currentMatrix[currentMode], glm::vec3(x, y, z));
//    multiplyToCurr(transMat);
}

void GLMatrix::Scale(float x, float y, float z) {
    glm::mat4 scaleMat = glm::scale(glm::vec3(x, y, z));
    multiplyToCurr(scaleMat);
}

void GLMatrix::SetModelMatrix(float *modelMat) {
    currentMatrix[ModelView] = glm::make_mat4(modelMat);
}

void GLMatrix::LoadIdentity() {
    currentMatrix[currentMode] = glm::mat4(1.0f);
}

void GLMatrix::Perspective(float fov, float aspect, float zNear, float zFar) {
    glm::mat4 pers = glm::perspective(fov, aspect, zNear, zFar);
    GLMatrix::multiplyToCurr(pers);
}

void GLMatrix::getCurrentMat(float *outMat){
    memcpy(outMat, glm::value_ptr(currentMatrix[currentMode]), sizeof(float)*16);
}
