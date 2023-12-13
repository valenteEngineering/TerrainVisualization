#version 330 core
out vec4 FragColor;

in vec3 vertexColor; // the input variable from the vertex shader (same name and same type)
in vec3 outNormal;
in vec3 fragPos;
in vec3 outLightPos;
in vec2 textCord;
uniform vec3 inLightPos;

uniform sampler2D terrainTexture;

void main()
{
    float ambientStriength = 0.1;
    vec3 ambient = ambientStriength * vec3(1,1,1);

    vec3 norm = normalize(outNormal);
    vec3 lightDir = normalize(inLightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1,1,1);


    vec4 particalColor = texture(terrainTexture, textCord);

    vec4 result = vec4((ambient + diffuse), 1.0) * particalColor;
    FragColor = result;
}