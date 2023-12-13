// A simple introductory program; its main window contains a static picture
// of a triangle, whose three vertices are red, green and blue.  The program
// illustrates viewing with default viewing parameters only.

#ifdef __APPLE_CC__
//#include <GLUT/glut.h>
#else

#include "Utils/Camera.h"
#include "Utils/GLShader.h"
#include "Utils/GLGeomShader.h"
#include "Data.h"
#include "Utils/GLMatrix.h"
#include "Utils/RenderableObject.h"
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <GL/glut.h>
#include <GL/glu.h>
#include <sys/time.h>
#include <ctime>
#include <chrono>
//#define STB_IMAGE_IMPLEMENTATION
#include "Utils/stb_image.h"

#endif

float angle = 0.0;
int frame = 0;


// MODIFIERS
float screenWidth = 2000;
float screenHeight = 1000;
float screenRatio = screenWidth / screenHeight;
int mousePos[2];
int leftMouseButton = 0; // 1 if pressed, 0 if not
int middleMouseButton = 0; // 1 if pressed, 0 if not
int rightMouseButton = 0; // 1 if pressed, 0 if not
bool keyStatus[10] = {false};

// TERRAIN DATA
//const char* terrainFile = "HeightMap/catalinaHR.png";
//const char *terrainFile = "HeightMap/sbHiRes.png";
//int terrainWidth = 13744;
//int terrainHeight = 10488;
const char *terrainFile = "HeightMap/sbLowRes.png";
int terrainWidth = 1648;
int terrainHeight = 1117;
// TERRAIN TEXTURE
const char *terrainTextureFile = "HeightMap/sbImage.png";
unsigned char *terrainTextureData;
int channels;


// CAMERA VARS
float cameraPosX = 650.0f;
float cameraPosY = 200.0f;
float cameraPosZ = 1550.0f;
Camera camera(glm::vec3(cameraPosX, cameraPosY, cameraPosZ));

// LIGHT VARS
float lightPosX = 150.0f;
float lightPosY = 300.0f;
float lightPosZ = 300.0f;

// DATA BUFFERS
float modelMat[16];
float perspectiveMat[16];
float viewMat[16];
// TERRAIN POINTS
GLuint VBO_TerrainPoints, VBO_TerrainColor, VBO_TerrainTriangleNormals, VBO_TerrainTextureUV, VBO_TerrainTexture, EBO_Terrain, VAO;
std::vector<GLfloat> terrainPoints;
std::vector<GLfloat> terrainPointColor;
std::vector<GLfloat> terrainLineColor;
std::vector<GLfloat> terrainTriangleColor;
std::vector<GLfloat> terrainTriangleNormalsFlat;
std::vector<GLfloat> terrainTriangleNormalsSmooth;
std::vector<GLfloat> terrainTexturePUV;

// GEOMETRY SHADER INFO
GLuint VBO_GeometryCube, VBO_GeometryCubeColor;
std::vector<GLfloat> cubePoints;
std::vector<GLfloat> cubeColor{0.0, 0.0, 1.0};


std::vector<GLuint> lineIndex;
std::vector<GLuint> triIndex;

// RENDER TYPES
enum RenderTypes {
    RenderPoints,
    RenderLines,
    RenderTriangle,
    RenderNormalTriangle
} activeRender = RenderPoints;

// SHADER VARS
GLMatrix *matrix;
GLShader *lightingShader;
GLShader *passthroughShader;
GLGeomShader *geometryShader;

RenderableObject *terrain;

void DivideVectorByScalar(std::vector<float> &v, float k) {
    for_each(v.begin(), v.end(), [k](float &c) { c /= k; });
}

void CrossProduct(std::vector<GLfloat> p1, std::vector<GLfloat> p2, std::vector<GLfloat> p3, std::vector<GLfloat> &outVector) {
    std::vector<GLfloat> v1(3);
    std::vector<GLfloat> v2(3);
    std::transform(p1.begin(), p1.end(), p3.begin(), v1.begin(), std::minus<GLfloat>());
    std::transform(p2.begin(), p2.end(), p3.begin(), v2.begin(), std::minus<GLfloat>());

    outVector[0] = v1[1] * v2[2] - v1[2] * v2[1];
    outVector[1] = -(v1[0] * v2[2] - v1[2] * v2[0]);
    outVector[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void reshape(int w, int h) {
    if (h == 0) {
        h = 1;
    }
    screenRatio = w * 1.0 / h;

    // SET PROJECTION, VIEWPORT, AND PERSPECTIVE
    glViewport(0, 0, w, h);
    matrix->SetMode(GLMatrix::ProjectionView);
    matrix->LoadIdentity();
    matrix->Perspective(45, screenRatio, 0.1, 5000.0);
    matrix->SetMode(GLMatrix::ModelView);
}

void renderPoints(GLShader *shader) {
    const void *offset = (const void *) nullptr;
    GLsizei stride = 0;
    GLboolean normalized = GL_FALSE;

    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainPoints);
    GLuint pos = glGetAttribLocation(shader->GetProgram(), "aPos");
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, normalized, stride, offset);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainColor);
    GLuint col = glGetAttribLocation(shader->GetProgram(), "inColor");
    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 3, GL_FLOAT, normalized, stride, offset);

    glDrawArrays(GL_POINTS, 0, terrainWidth * terrainHeight);
    glDisableVertexAttribArray(pos);
    glDisableVertexAttribArray(col);
}

void renderLines(GLShader *shader) {
    const void *offset = (const void *) nullptr;
    GLsizei stride = 3 * (sizeof(GLfloat));
    GLboolean normalized = GL_FALSE;

    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainPoints);
    GLuint pos = glGetAttribLocation(shader->GetProgram(), "aPos");
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, normalized, stride, offset);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainColor);
    GLuint col = glGetAttribLocation(shader->GetProgram(), "inColor");
    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 3, GL_FLOAT, normalized, stride, offset);

    glDrawElements(GL_LINES, lineIndex.size(), GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(pos);
    glDisableVertexAttribArray(col);
}

void renderTriangles(GLShader *shader) {
    const void *offset = (const void *) nullptr;
    GLsizei stride = 0;
    GLboolean normalized = GL_FALSE;

    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainPoints);
    GLuint pos = glGetAttribLocation(shader->GetProgram(), "aPos");
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, normalized, stride, offset);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainColor);
    GLuint col = glGetAttribLocation(shader->GetProgram(), "inColor");
    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 3, GL_FLOAT, normalized, stride, offset);

    glDrawElements(GL_TRIANGLES, triIndex.size(), GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(pos);
    glDisableVertexAttribArray(col);
}

void renderTrianglesWithNormals(GLShader *shader) {
    const void *offset = (const void *) nullptr;
    GLsizei stride = 0;
    GLboolean normalized = GL_FALSE;

    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainPoints);
    GLuint pos = glGetAttribLocation(shader->GetProgram(), "aPos");
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, normalized, stride, offset);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainColor);
    GLuint col = glGetAttribLocation(shader->GetProgram(), "inColor");
    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 3, GL_FLOAT, normalized, stride, offset);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainTriangleNormals);
    GLuint normal = glGetAttribLocation(shader->GetProgram(), "inNormal");
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(normal, 3, GL_FLOAT, normalized, stride, offset);

    glDrawElements(GL_TRIANGLES, triIndex.size(), GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(pos);
    glDisableVertexAttribArray(col);
}

void translate(GLShader *currentShader) {
    // SET UP VIEW MAT
    camera.GetViewMatrix(viewMat);
    currentShader->SetView(viewMat);

    // SET UP MODEL MAT
    matrix->SetMode(GLMatrix::ModelView);
    matrix->LoadIdentity();
    matrix->getCurrentMat(modelMat);
    currentShader->SetModel(modelMat);

    // SET UP PERSPECTIVE MODEL
    matrix->SetMode(GLMatrix::ProjectionView);
    matrix->LoadIdentity();
    matrix->Perspective(glm::radians(camera.Zoom), screenRatio, 0.1, 2000.0);
    matrix->getCurrentMat(perspectiveMat);
    currentShader->SetPerspective(perspectiveMat);
}

void display() {
    frame++;
    if (frame % 1000 == 0) {
        std::cout << "now frame: " << frame << std::endl;
        angle += 0.1;
    }
    std::cout << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << std::endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    lightingShader->SetProgram();
//    passthroughShader->SetProgram();
    translate(lightingShader);
    lightingShader->SetLightPos(lightPosX, lightPosY, lightPosZ);
//    lightingShader->SetViewPos(camera.Position[0], camera.Position[1], camera.Position[2]); // THIS DOESNT DO ANYTHING RN

    glBindTexture(GL_TEXTURE_2D, VBO_TerrainTexture);
    if (activeRender == RenderPoints) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainTextureUV);
        GLuint textureUV = glGetAttribLocation(lightingShader->GetProgram(), "inTextCord");
        glEnableVertexAttribArray(textureUV);
        glVertexAttribPointer(textureUV, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        renderPoints(lightingShader);
    }
    if (activeRender == RenderLines) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainTextureUV);
        GLuint textureUV = glGetAttribLocation(lightingShader->GetProgram(), "inTextCord");
        glEnableVertexAttribArray(textureUV);
        glVertexAttribPointer(textureUV, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        renderLines(lightingShader);
    }
    if (activeRender == RenderTriangle) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainTextureUV);
        GLuint textureUV = glGetAttribLocation(lightingShader->GetProgram(), "inTextCord");
        glEnableVertexAttribArray(textureUV);
        glVertexAttribPointer(textureUV, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        renderTriangles(lightingShader);
    }
    if (activeRender == RenderNormalTriangle) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainTextureUV);
        GLuint textureUV = glGetAttribLocation(lightingShader->GetProgram(), "inTextCord");
        glEnableVertexAttribArray(textureUV);
        glVertexAttribPointer(textureUV, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        renderTrianglesWithNormals(lightingShader);
    }

    // DRAW SINGLE CUBE FROM GEOMETRY SHADER


    // RENDER LIGHT
    matrix->SetMode(GLMatrix::ModelView);
    matrix->LoadIdentity();
    matrix->Translate(lightPosX, lightPosY, lightPosZ);
    matrix->getCurrentMat(modelMat);
    lightingShader->SetModel(modelMat);
    gluSphere(gluNewQuadric(), 25, 100, 20);


    geometryShader->SetProgram();
    translate(geometryShader);
    matrix->SetMode(GLMatrix::ModelView);
    matrix->Rotate(angle, 1, 0, 0);
//    matrix->Translate(150.0, 300.0, 300.0);
    matrix->getCurrentMat(modelMat);
    geometryShader->SetModel(modelMat);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_GeometryCube);
    GLuint pos = glGetAttribLocation(geometryShader->GetProgram(), "aPos");
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_GeometryCubeColor);
    GLuint col = glGetAttribLocation(geometryShader->GetProgram(), "inColor");
    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glDrawArrays(GL_POINTS, 0, 1);

    glutSwapBuffers();
}

void idle() {
}

void initVBO() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    // PER POINT LOCATION
    glGenBuffers(1, &VBO_TerrainPoints);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainPoints);
    glBufferData(GL_ARRAY_BUFFER, terrainPoints.size() * sizeof(float), &terrainPoints[0], GL_STATIC_DRAW);
    // PER POINT COLOR
    glGenBuffers(1, &VBO_TerrainColor);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainColor);
    glBufferData(GL_ARRAY_BUFFER, terrainPointColor.size() * sizeof(float), &terrainPointColor[0], GL_STATIC_DRAW);
    // PER POINT TEXTURE UV
    glGenBuffers(1, &VBO_TerrainTextureUV);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainTextureUV);
    glBufferData(GL_ARRAY_BUFFER, terrainTexturePUV.size() * sizeof(float), &terrainTexturePUV[0], GL_STATIC_DRAW);

    // GEN NORMALS VBO, BUT DON'T SET DATA BECAUSE THAT IS SETTING DEPENDENT
    glGenBuffers(1, &VBO_TerrainTriangleNormals);
    // ELEMENT LOCATION ARRAY, DON'T SET DATA BECAUSE DEPENDENT
    glGenBuffers(1, &EBO_Terrain);

    // TEXTURE DATA
    glGenTextures(1, &VBO_TerrainTexture);
    glBindTexture(GL_TEXTURE_2D, VBO_TerrainTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, terrainWidth, terrainHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, terrainTextureData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // GEO CUBE DATA
    glGenBuffers(1, &VBO_GeometryCube);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_GeometryCube);
    glBufferData(GL_ARRAY_BUFFER, cubePoints.size() * sizeof(float), &cubePoints[0], GL_STATIC_DRAW);

    glGenBuffers(1, &VBO_GeometryCubeColor);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_GeometryCubeColor);
    glBufferData(GL_ARRAY_BUFFER, cubeColor.size() * sizeof(float), &cubeColor[0], GL_STATIC_DRAW);
}

void loadDataFromTerrain() {
    int w = terrain->GetWidth();
    int h = terrain->GetHeight();

    std::vector<GLfloat> pointMain(3);
    std::vector<GLfloat> pointMain_x(3);
    std::vector<GLfloat> pointMain_y(3);
    std::vector<GLfloat> pointUp(3);
    std::vector<GLfloat> pointUp_x(3);
    std::vector<GLfloat> pointUp_y(3);
    std::vector<GLfloat> pointRight(3);
    std::vector<GLfloat> pointRight_x(3);
    std::vector<GLfloat> pointRight_y(3);
    std::vector<GLfloat> pointRightUp(3);
    std::vector<GLfloat> pointRightUp_x(3);
    std::vector<GLfloat> pointRightUp_y(3);

    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    // GRAB ALL POINTS FROM TERRAIN
    terrain->GetPoints(terrainPoints);
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            int indexMain = 3 * (i * h + j);
            int indexMain_x = 3 * ((i + 1) * h + j);
            int indexMain_y = 3 * (i * h + (j + 1));
            int indexRight = 3 * (i * h + (j + 1));
            int indexRight_x = 3 * ((i + 1) * h + (j + 1));
            int indexRight_y = 3 * (i * h + (j + 1 + 1));
            int indexUp = 3 * ((i + 1) * h + j);
            int indexUp_x = 3 * ((i + 1 + 1) * h + j);
            int indexUp_y = 3 * ((i + 1) * h + (j + 1));
            int indexRightUp = 3 * ((i + 1) * h + (j + 1));
            int indexRightUp_x = 3 * ((i + 1 + 1) * h + (j + 1));
            int indexRightUp_y = 3 * ((i + 1) * h + (j + 1 + 1));

            //main
            std::copy(terrainPoints.begin() + indexMain, terrainPoints.begin() + indexMain + 3, pointMain.begin());
            if (i < w - 1) {
                std::copy(terrainPoints.begin() + indexMain_x, terrainPoints.begin() + indexMain_x + 3, pointMain_x.begin());
            }
            if (j < h - 1) {
                std::copy(terrainPoints.begin() + indexMain_y, terrainPoints.begin() + indexMain_y + 3, pointMain_y.begin());
            }

            if (i < w - 1) {
                //up
                std::copy(terrainPoints.begin() + indexUp, terrainPoints.begin() + indexUp + 3, pointUp.begin());
                if (i < w - 2) {
                    //up x mod
                    std::copy(terrainPoints.begin() + indexUp_x, terrainPoints.begin() + indexUp_x + 3, pointUp_x.begin());
                }
                if (j < h - 1) {
                    //up y mod
                    std::copy(terrainPoints.begin() + indexUp_y, terrainPoints.begin() + indexUp_y + 3, pointUp_y.begin());
                }
            }

            if (j < h - 1) {
                //right
                std::copy(terrainPoints.begin() + indexRight, terrainPoints.begin() + indexRight + 3, pointRight.begin());
                if (i < w - 1) {
                    // right x mod
                    std::copy(terrainPoints.begin() + indexRight_x, terrainPoints.begin() + indexRight_x + 3, pointRight_x.begin());
                }
                if (j < h - 2) {
                    // right y mod
                    std::copy(terrainPoints.begin() + indexRight_y, terrainPoints.begin() + indexRight_y + 3, pointRight_y.begin());
                }
            }

            if (i < w - 1 && j < h - 1) {
                //up-right
                std::copy(terrainPoints.begin() + indexRightUp, terrainPoints.begin() + indexRightUp + 3, pointRightUp.begin());
                if (i < w - 2 && j < h - 1) {
                    //up-right x mod
                    std::copy(terrainPoints.begin() + indexRightUp_x, terrainPoints.begin() + indexRightUp_x + 3, pointRightUp_x.begin());
                }
                if (i < w - 1 && j < h - 2) {
                    //up-right y mod
                    std::copy(terrainPoints.begin() + indexRightUp_y, terrainPoints.begin() + indexRightUp_y + 3, pointRightUp_y.begin());
                }
            }

            // TERRAIN TEXTURE CORDS
            terrainTexturePUV.push_back((float) i / (float) terrainWidth);
            terrainTexturePUV.push_back((float) j / (float) terrainHeight);
            // SET POINT COLORS
            terrainPointColor.insert(terrainPointColor.end(), 3, terrainPoints[indexMain + 1] + 50);
            // SET LINE DATA
            if (i < w - 1) {
                lineIndex.push_back(indexMain / 3);
                lineIndex.push_back(indexUp / 3);
            }
            if (j < h - 1) {
                lineIndex.push_back(indexMain / 3);
                lineIndex.push_back(indexRight / 3);
            }

            // SET TRIANGLE DATA
            if (i < w - 1 && j < h - 1) {
                //CALCULATE SURFACE NORMAL OF THIS TRIANGLE
                std::vector<GLfloat> crossProduct(3);
                CrossProduct(pointMain, pointRight, pointUp, crossProduct);
                terrainTriangleNormalsFlat.insert(terrainTriangleNormalsFlat.end(), crossProduct.begin(), crossProduct.end());
//                terrainTriangleNormalsFlat.insert(terrainTriangleNormalsFlat.end(), crossProduct.begin(), crossProduct.end());
//                terrainTriangleNormalsFlat.insert(terrainTriangleNormalsFlat.end(), crossProduct.begin(), crossProduct.end());
                // CALCULATE SMOOTH NORMAL OF THIS TRIANGLE
                CrossProduct(pointMain_y, pointMain_x, pointMain, crossProduct);
                terrainTriangleNormalsSmooth.insert(terrainTriangleNormalsSmooth.end(), crossProduct.begin(), crossProduct.end());
                CrossProduct(pointRight_y, pointRight_x, pointRight, crossProduct);
                terrainTriangleNormalsSmooth.insert(terrainTriangleNormalsSmooth.end(), crossProduct.begin(), crossProduct.end());
                CrossProduct(pointUp_y, pointUp_x, pointUp, crossProduct);
                terrainTriangleNormalsSmooth.insert(terrainTriangleNormalsSmooth.end(), crossProduct.begin(), crossProduct.end());

                triIndex.push_back(indexMain / 3);
                triIndex.push_back(indexRight / 3);
                triIndex.push_back(indexUp / 3);
            } else {
                std::vector<float> crossProduct{0, 1, 0};
                terrainTriangleNormalsFlat.insert(terrainTriangleNormalsFlat.end(), crossProduct.begin(), crossProduct.end());
            }
            if (i < w - 1 && j < h - 1) {
                // CALCULATE SURFACE NORMAL OF THIS TRIANGLE
                std::vector<GLfloat> crossProduct(3);
                CrossProduct(pointUp, pointRight, pointRightUp, crossProduct);
//                terrainTriangleNormalsFlat.insert(terrainTriangleNormalsFlat.end(), crossProduct.begin(), crossProduct.end());
//                terrainTriangleNormalsFlat.insert(terrainTriangleNormalsFlat.end(), crossProduct.begin(), crossProduct.end());
//                terrainTriangleNormalsFlat.insert(terrainTriangleNormalsFlat.end(), crossProduct.begin(), crossProduct.end());
                // CALCULATE SMOOTH NORMAL OF THIS TRIANGLE
                CrossProduct(pointUp_y, pointUp_x, pointUp, crossProduct);
                terrainTriangleNormalsSmooth.insert(terrainTriangleNormalsSmooth.end(), crossProduct.begin(), crossProduct.end());
                CrossProduct(pointRight_y, pointRight_x, pointRight, crossProduct);
                terrainTriangleNormalsSmooth.insert(terrainTriangleNormalsSmooth.end(), crossProduct.begin(), crossProduct.end());
                CrossProduct(pointRightUp_y, pointRightUp_x, pointRightUp, crossProduct);
                terrainTriangleNormalsSmooth.insert(terrainTriangleNormalsSmooth.end(), crossProduct.begin(), crossProduct.end());

                triIndex.push_back(indexUp / 3);
                triIndex.push_back(indexRight / 3);
                triIndex.push_back(indexRightUp / 3);
            }
        }
    }
    auto later = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << "time taken: " << later - now << std::endl;
    // DIVIDE BY 255 TO NORMALIZE COLORS
    DivideVectorByScalar(terrainPointColor, 255.0f);
    DivideVectorByScalar(terrainLineColor, 255.0f);
    DivideVectorByScalar(terrainTriangleColor, 255.0f);
}

void initScene() {
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);

    // CONTAINER AND CONTROLLER FOR MODEL/PERSPECTIVE MATRIX
    matrix = new GLMatrix();

    // LOAD AND COMPILE ALL SHADERS, SET SHADING PROGRAM  TODO: MAKE SHADER/PIPELINE IT'S OWN CLASS, USE INHARITANCE
    passthroughShader = new GLShader();
    lightingShader = new GLShader();
    geometryShader = new GLGeomShader();
    passthroughShader->init("Shaders/passthrough.vert", "Shaders/passthrough.frag");
    lightingShader->init("Shaders/initLighting.vert", "Shaders/initLighting.frag");
    geometryShader->init("Shaders/geoCube.vert", "Shaders/geoCube.geom", "Shaders/geoCube.frag");

    // LOAD TERRAIN
    terrain = new RenderableObject();
    terrain->LoadData(terrainFile, terrainWidth, terrainHeight, 0.5);
    loadDataFromTerrain();

    // LOAD GEO-CUBE
    cubePoints.push_back(0.0); // X
    cubePoints.push_back(0.0); // Y
    cubePoints.push_back(0.0); // Z

    // LOAD TERRAIN TEXTURE
    terrainTextureData = stbi_load((const char *) terrainTextureFile, &terrainWidth, &terrainHeight, &channels, 4);
    // LOAD TERRAIN DATA INTO VBO
    initVBO();
    // FREE TEXTURE DATA FROM WORKING MEMORY
    stbi_image_free(terrainTextureData);
}

void keyboardHandler(unsigned char key, int x, int y) {
    float speed = 10.0f;

    switch (key) {
        // CAMERA LOCATION
        case 'o':
            camera.ProcessKeyboard(FORWARD, speed);
            break;
        case 'q':
            camera.ProcessKeyboard(BACKWARD, speed);
            break;
        case ';':
            camera.ProcessKeyboard(LEFT, speed);
            break;
        case 'j':
            camera.ProcessKeyboard(RIGHT, speed);
            break;

//        RENDER MODE
        case '1':
            activeRender = RenderPoints;
            break;
        case '2':
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Terrain);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, lineIndex.size() * sizeof(GLuint), &lineIndex[0], GL_STATIC_DRAW);
            activeRender = RenderLines;
            break;
        case '3':
            activeRender = RenderTriangle;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Terrain);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, triIndex.size() * sizeof(GLuint), &triIndex[0], GL_STATIC_DRAW);
            break;
        case '4':
            activeRender = RenderNormalTriangle;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Terrain);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, triIndex.size() * sizeof(GLuint), &triIndex[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainTriangleNormals);
            glBufferData(GL_ARRAY_BUFFER, terrainTriangleNormalsFlat.size() * sizeof(float), &terrainTriangleNormalsFlat[0], GL_STATIC_DRAW);
            break;
        case '5':
            activeRender = RenderNormalTriangle;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Terrain);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, triIndex.size() * sizeof(GLuint), &triIndex[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_TerrainTriangleNormals);
            glBufferData(GL_ARRAY_BUFFER, terrainTriangleNormalsSmooth.size() * sizeof(float), &terrainTriangleNormalsSmooth[0], GL_STATIC_DRAW);

            // LIGHT VARS
        case 'm':
            lightPosZ += 10;
            break;
        case 'w':
            lightPosZ -= 10;
    }
}

void lightMovement(int key, int x, int y) {
    switch (key) {
        //LIGHT LOCATION
        case GLUT_KEY_UP:
            lightPosY += 10;
            break;
        case GLUT_KEY_DOWN:
            lightPosY -= 10;
            break;
        case GLUT_KEY_LEFT:
            lightPosX += 10;
            break;
        case GLUT_KEY_RIGHT:
            lightPosX -= 10;
            break;
    }
}

void mouseLocationHandler(int x, int y) {
    mousePos[0] = x;
    mousePos[1] = y;
}

void mouseButtonHandler(int button, int state, int x, int y) {
    switch (button) {
        case GLUT_LEFT_BUTTON:
            leftMouseButton = (state == GLUT_DOWN);
            break;
        case 3: // SCROLL WHEEL UP
            camera.ProcessMouseScroll(1);
            break;
        case 4: // SCROLL WHEEL DOWN
            camera.ProcessMouseScroll(-1);
            break;
    }
    mousePos[0] = x;
    mousePos[1] = y;
}

void mouseDragHandler(int x, int y) {
    int mouseDela[2] = {x - mousePos[0], y - mousePos[1]};

    camera.ProcessMouseMovement((float) mouseDela[0], (float) mouseDela[1]);
    mousePos[0] = x;
    mousePos[1] = y;
}


// Initializes GLUT, the display mode, and main window; registers callbacks;
// enters the main event loop.
int main(int argc, char **argv) {
//     INITIALIZE  GLUT AND OPEN NEW WINDOW
    std::cout << "init glut" << std::endl;
    glutInit(&argc, argv);
    std::cout << "init opengl" << std::endl;
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Terrain");

    // GLUT CALL BACK FUNCTIONS
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutSpecialFunc(lightMovement);

    glutMotionFunc(mouseDragHandler);
    glutPassiveMotionFunc(mouseLocationHandler);
    glutMouseFunc(mouseButtonHandler);
    glutKeyboardFunc(keyboardHandler);

    // INITIALIZE GLEW, LOAD AND COMPILE ALL SHADERS, SET SHADING PROGRAM
    GLenum result = glewInit();
    if (result != GLEW_OK) {
        std::cout << "error: " << glewGetErrorString(result) << std::endl;
        exit(EXIT_FAILURE);
    }
    initScene();

    // SINK INTO MAIN GLUT LOOP
    glutMainLoop();
    return 1;
}