#version 330 core
out vec4 FragColor;

in float outColor;

void main(){
    FragColor = mix(vec4(1, 1, 1, 1), vec4(0, 0, 0, 1), outColor);
}