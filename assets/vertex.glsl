#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexcoord;
layout (location = 3) in vec2 vColor;
layout (location = 4) in vec4 vJoint;
layout (location = 5) in vec4 vWeight;

out float outColor;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 jointMat[50];

void main(){
    gl_Position = projection * view * model * vec4(vPosition.x + vOffset.x, vPosition.y + vOffset.y, vPosition.z , 1);
    texCoord = vTexcoord;
}