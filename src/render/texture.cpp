#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "texture.h"

GLuint texture;

void texture_create(const char* path) {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST_MIPMAP_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// load img
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true); // match opengl orientation
	unsigned char *img = stbi_load(path, &width, &height, &channels, 0);

	// für atlas nur rgba nötig
	GLenum internalFormat, dataFormat;
	if (channels == 4) 		{ internalFormat = GL_RGBA8; dataFormat = GL_RGBA; }
	else if (channels == 3) { internalFormat = GL_RGB8; dataFormat = GL_RGB; }
	else 					{ internalFormat = GL_RED; dataFormat = GL_RED; }

	if (img) {
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, img);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else std::cerr << "Error: stbi load failed: " << path << std::endl;

	stbi_image_free(img);
}
