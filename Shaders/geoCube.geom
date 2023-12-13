#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 7) out;



in VS_OUT{
    vec3 color;
}gs_in[];
out vec3 fragColor;

void buildCube(vec4 loc){
    fragColor = gs_in[0].color;

    gl_Position = (loc + vec4(-1.0, -1.0, -1.0, 0.0));
    EmitVertex();

    gl_Position = (loc + vec4(1.0, -1.0, -1.0, 0.0));
    EmitVertex();

    gl_Position = (loc + vec4(1.0, 1.0, -1.0, 0.0));
    EmitVertex();

    gl_Position = (loc + vec4(-1.0, 1.0, -1.0, 0.0));
    EmitVertex();

    gl_Position = (loc + vec4(-1.0, 1.0, 1.0, 0.0));
    EmitVertex();

    gl_Position = (loc + vec4(-1.0, -1.0, 1.0, 0.0));
    EmitVertex();

    gl_Position = (loc + vec4(1.0, -1.0, 1.0, 0.0));
    EmitVertex();

    EndPrimitive();
}

void main(){
    buildCube(gl_in[0].gl_Position);
}


