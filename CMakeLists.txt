cmake_minimum_required(VERSION 3.23)
project(untitled)
set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

add_executable(untitled main.cpp Data.h Utils/GLShader.cpp Utils/GLShader.h Utils/GLMatrix.cpp Utils/GLMatrix.h Utils/stb_image.h Utils/RenderableObject.cpp Utils/RenderableObject.h Utils/Camera.h Utils/GLGeomShader.cpp Utils/GLGeomShader.h)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${FREEGLUT_INCLUDE_DIRS})
target_link_libraries(untitled ${FREEGLUT_LIBRARIES})
target_link_libraries(untitled ${OPENGL_LIBARIES})
target_link_libraries(untitled ${GLUT_LIBRARY})
target_link_libraries(untitled ${GLFW_LIBRARY})
target_link_libraries(untitled ${GLEW_LIBRARY})
target_link_libraries(untitled -lopengl32 -lfreeglut -lglfw3 -lglew32 -lglu32)
