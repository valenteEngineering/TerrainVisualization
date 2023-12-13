//
// Created by alexv on 10/21/2022.
//

#include "GLShader.h"

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>


GLShader::GLShader() : h_shaderProgram(0) {}

GLShader::~GLShader() {
    glDeleteProgram(h_shaderProgram);
    glDeleteShader(h_shaderProgram);
}

void GLShader::init(const char *vertexPath, const char *fragmentPath) {
    GLShader::LoadShader(vertexPath, fragmentPath);
    GLShader::SetMatrixHandels();
}

GLuint GLShader::LoadShader(const char *vertexPath, const char *fragmentPath) {
    h_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    h_fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    //TODO MAKE BETTER WITH DYNAMIC MEM ALLOCATION WITH FILE SIZE
    char *vertShaderCode = new char[128 * 1024];
    char *fragShaderCode = new char[128 * 1024];

    ReadFile(vertexPath, vertShaderCode, 128 * 1024);
    ReadFile(fragmentPath, fragShaderCode, 128 * 1024);

    h_vertexShader = CompileShader(h_vertexShader, vertShaderCode);
    h_fragShader = CompileShader(h_fragShader, fragShaderCode);

    return CreateProgram(h_vertexShader, h_fragShader);
//    return h_shaderProgram;
}
int GLShader::ReadFile(const char *filename, char *code, int len) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
        return 1;

    code[fread(code, 1, len, file)] = '\0';
    fclose(file);

    return 0;
}
GLuint GLShader::CreateProgram(GLuint vertShader, GLuint fragShader) {
    GLint result = GL_FALSE;
    int logLength;

    std::cout << "Linking h_shaderProgram" << std::endl;
    h_shaderProgram = glCreateProgram();
    glAttachShader(h_shaderProgram, vertShader);
    glAttachShader(h_shaderProgram, fragShader);
    glLinkProgram(h_shaderProgram);

    glGetProgramiv(h_shaderProgram, GL_LINK_STATUS, &result);
    glGetProgramiv(h_shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> programError((logLength > 1) ? logLength : 1);
    glGetProgramInfoLog(h_shaderProgram, logLength, nullptr, &programError[0]);
    std::cout << &programError[0] << std::endl;

//    glDeleteShader(vertShader);
//    glDeleteShader(fragShader);

    return h_shaderProgram;
}
GLuint GLShader::CompileShader(GLuint shader, const char *shaderSrc) {
    GLint result = GL_FALSE;
    int logLength;

    const char *shaderCode[] = {shaderSrc};
    GLint codeLength[] = {(GLint) strlen(shaderSrc)};
    // Compile vertex passthroughShader
    std::cout << "Compiling passthroughShader." << std::endl;
    glShaderSource(shader, 1, shaderCode, codeLength);
    glCompileShader(shader);

    // Check vertex passthroughShader
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> shaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(shader, logLength, nullptr, &shaderError[0]);
    std::cout << &shaderError[0] << std::endl;

    return shader;
}
GLuint GLShader::GetProgram(){
    return h_shaderProgram;
}
void GLShader::SetView(float *m){
    glUniformMatrix4fv(h_view, 1, GL_FALSE, m);
}
void  GLShader::SetModel(float *m) {
    glUniformMatrix4fv(h_model, 1, GL_FALSE, m);
}
void GLShader::SetPerspective(float *p) {
    glUniformMatrix4fv(h_perspective, 1, GL_FALSE, p);
}
void GLShader::SetProgram(){
    glUseProgram(h_shaderProgram);
}

const void GLShader::SetLightPos(float x, float y, float z) {
    glUniform3f(h_lightPos, x, y, z);
}

const void GLShader::SetViewPos(float x, float y, float z){
    glUniform3f(h_viewPos, x, y, z);
}


void GLShader::SetMatrixHandels() {
    h_model = glGetUniformLocation(h_shaderProgram, "model");
    h_perspective = glGetUniformLocation(h_shaderProgram, "perspective");
    h_view = glGetUniformLocation(h_shaderProgram, "view");
    h_lightPos = glGetUniformLocation(h_shaderProgram, "inLightPos");
}