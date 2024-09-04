#version 460 core

layout(location = 0) in vec2 Position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(){
    mat4 mvp = projectionMatrix * modelMatrix;
    vec4 loc = vec4(mvp * vec4(Position, 0.0, 1.0));
    gl_Position = loc;
    // gl_Position =  vec4(Position, 0.0, 1.0);
}