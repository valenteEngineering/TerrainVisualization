//
// Created by alexv on 10/31/2022.
//

#ifndef UNTITLED_GLGEOMSHADER_H
#define UNTITLED_GLGEOMSHADER_H

#include "GLShader.h"

class GLGeomShader : public GLShader{
public:
    void init(const char *vertexPath, const char *geometryPath,const char *fragmentPath);
    void loadGeometryShader(const char *geometryPath);

protected:
    GLint h_geometryShader = -1;
};

//
#endif //UNTITLED_GLGEOMSHADER_H
