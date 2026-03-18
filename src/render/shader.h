#ifndef SHADER_H
#define SHADER_H

#include "../util/util.h"

inline GLuint _load(GLuint type, const std::string &path) {
	// read file
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Error: couldnt read file " << path << std::endl;
		return 0;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	std::string text = buffer.str();
	const char* src = text.c_str();

	// compile shader
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cerr << "Error: shader compilation failed: " << path << "\n" << infoLog << std::endl;

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

inline GLuint shader_create(const std::string &vsPath, const std::string &fsPath) {
	GLuint vs = _load(GL_VERTEX_SHADER, vsPath);
	GLuint fs = _load(GL_FRAGMENT_SHADER, fsPath);
	GLuint program = glCreateProgram();

	// attach shader objects to program
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		char infoLog[512];
		glGetProgramInfoLog(program, 512, nullptr, infoLog);
		std::cerr << "Error: shader program linking failed\n" << infoLog << std::endl;

		glDeleteProgram(program);
		return 0;
	}

	// glDetachShader(program, vs);
	// glDetachShader(program, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}

#endif
