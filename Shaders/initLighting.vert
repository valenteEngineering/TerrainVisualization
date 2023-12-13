#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
in vec3 inColor;
in vec3 inNormal;
in vec2 inTextCord;


uniform mat4 perspective;
uniform mat4 model;
uniform mat4 view;
uniform vec3 inLightPos;

out vec3 vertexColor; // specify a color output to the fragment shader
out vec3 outNormal;
out vec3 fragPos;
out vec3 outLightPos;
out vec2 textCord;

void main()
{
    outNormal = inNormal;
    fragPos = vec3(model * vec4(aPos, 1.0));
    vertexColor = vec3(inColor); // set the output variable to a dark-red color
    outLightPos = inLightPos;
    textCord = inTextCord;

    gl_Position = perspective *  view * vec4(fragPos, 1.0); // see how we directly give a vec3 to vec4's constructor
}