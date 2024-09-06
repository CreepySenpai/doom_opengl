#version 460 core

layout(location = 0) in vec3 Position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(){
    mat4 mvpMatrix = projectionMatrix * viewMatrix * modelMatrix;
    gl_Position = mvpMatrix * vec4(Position, 1.0);
}