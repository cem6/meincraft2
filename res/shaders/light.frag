#version 330 core
out vec4 FragColor;

// get form vertex shader
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D ourTexture;
uniform vec3 lightPos;
uniform vec3 camPos;

vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main() {
	// ambient
	float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	float specularStrength = 0.2;
	vec3 viewDir = normalize(camPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * texture(ourTexture, TexCoord).rgb;

    // FragColor = vec4(ourColor, 1.0);
	// FragColor = texture(ourTexture, TexCoord);
	FragColor = vec4(vec3(result), 1.0);
}
