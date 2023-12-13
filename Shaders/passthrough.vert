#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
in vec3 inColor;

uniform mat4 perspective;
uniform mat4 model;
uniform mat4 view;

out vec4 vertexColor; // specify a color output to the fragment shader

void main()
{
	gl_Position = perspective * model * view * vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
	vertexColor = vec4(inColor, 1.0); // set the output variable to a dark-red color
}