#version 450

layout(location = 0) in vec3 p;
layout(location = 1) in vec2 texCoord;

uniform mat4 u_MVPMatrix;

out vec2 st;

void main() {  
    //gl_Position = u_MVPMatrix * vec4(p, 1.0);  
	gl_Position = vec4(p, 1.0);  
	st = texCoord;
} 