#version 330 core
// get from code
layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aColor;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

// pass to fragment shader
// out vec3 ourColor; 
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
// out vec3 WorldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main() {
	// pass to fragment shader
    // ourColor = aColor;
	TexCoord = aTexCoord;
	// Normal = mat3(transpose(inverse(model))) * aNormal; // do on cpu
	Normal = aNormal;
	FragPos = vec3(model * vec4(aPos, 1.0));
	// WorldPos = aPos;

	vec3 pos = aPos;
	// waves
	// float wave = sin(pos.x * 2.0 + time * 1.5) * 0.2;
	// wave += cos(pos.z * 1.5 + time * 1.0) * 0.1;
	// pos.y += wave;

    gl_Position = projection * view * model * vec4(pos, 1.0);
}
