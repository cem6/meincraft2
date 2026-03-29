#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;
// vec4 bgColor = vec4(1.0, 0.0, 0.0, 1);

void main() {
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
	color = vec4(textColor, 1.0) * sampled;

	// float alpha = texture(text, TexCoords).r;
	// vec4 textPart = vec4(textColor, 1.0) * alpha;
	// vec4 bgPart = bgColor * (1.0 - alpha);
	// color = textPart + bgPart;
}
