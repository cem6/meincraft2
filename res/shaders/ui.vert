#version 330 core
layout (location = 0) in vec3 aPos;

uniform float aspect;

void main() {
	gl_Position = vec4(aPos.x / aspect, aPos.y, aPos.z , 1.0);
}
