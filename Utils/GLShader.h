//
// Created by alexv on 10/21/2022.
//

#ifndef UNTITLED_GLSHADER_H
#define UNTITLED_GLSHADER_H

#include <string>
#include "GL/glew.h"

class GLShader {
public:
    GLShader();
    ~GLShader();
    void init(const char *vertexPath, const char *fragmentPath);
    GLuint GetProgram();
    void SetView(float *m);
    void SetModel(float* m);
    void SetPerspective(float* p);
    void SetProgram();
    const void SetLightPos(float x, float y, float z);
    const void SetViewPos(float x, float y, float z);

protected:
    GLint h_shaderProgram = -1;
    GLint h_vertexShader = -1;
    GLint h_fragShader = -1;
    GLint h_model = -1;
    GLint h_perspective = -1;
    GLint h_view = -1;
    GLint h_lightPos = -1;
    GLint h_viewPos = -1;

    GLuint LoadShader(const char *vertexPath, const char *fragmentPath);
    int ReadFile(const char *filename, char *code, int len);
    GLuint CompileShader(GLuint shader, const char *shaderSrc);
    GLuint CreateProgram(GLuint vertShader, GLuint fragShader);
    void SetMatrixHandels();
};


#endif //UNTITLED_GLSHADER_H
