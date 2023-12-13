//
// Created by alexv on 10/31/2022.
//

#include "GLGeomShader.h"
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>

void GLGeomShader::init(const char *vertexPath, const char *geometryPath, const char *fragmentPath) {
    GLShader::init(vertexPath, fragmentPath);
    GLGeomShader::loadGeometryShader(geometryPath);
}

void GLGeomShader::loadGeometryShader(const char *geometryPath) {
    GLint result = GL_FALSE;
    int logLength;

    h_geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

    char *geomShaderCode = new char[128 * 1024];
    ReadFile(geometryPath, geomShaderCode, 128 * 1024);

    h_geometryShader = CompileShader(h_geometryShader, geomShaderCode);
    glAttachShader(h_shaderProgram, h_geometryShader);
    glAttachShader(h_shaderProgram, h_vertexShader);
    glAttachShader(h_shaderProgram, h_fragShader);
    glLinkProgram(h_shaderProgram);

    glGetProgramiv(h_shaderProgram, GL_LINK_STATUS, &result);
    glGetProgramiv(h_shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> programError((logLength > 1) ? logLength : 1);
    glGetProgramInfoLog(h_shaderProgram, logLength, nullptr, &programError[0]);
    std::cout << &programError[0] << std::endl;

    glDeleteShader(h_geometryShader);
    glDeleteShader(h_vertexShader);
    glDeleteShader(h_fragShader);

}
