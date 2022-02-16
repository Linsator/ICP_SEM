#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
// Normals (not necessarily normalized)
layout (location = 2) in vec3 aNormal;

uniform mat4 uProj_m, uMV_m;

out vec2 texcoord;

out vec3 crntPos;

// Outputs the normal for the Fragment Shader
out vec3 Normal;

void main()
{
    // calculates current position
	crntPos = vec3(uMV_m * vec4(aPos, 1.0f));
    // Outputs the positions/coordinates of all vertices
    gl_Position = uProj_m * uMV_m * vec4(aPos, 1.0f);
    texcoord = aTex;
    Normal = aNormal;
}