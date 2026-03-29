#version 330 core
out vec4 FragColor;

// get form vertex shader
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
// in vec3 WorldPos;

uniform sampler2D ourTexture;
uniform vec3 lightPos;
uniform vec3 camPos;
uniform ivec3 aimPos;
uniform ivec3 aimNorm;

vec3 lightColor = vec3(1.0, 0.98, 0.9);
vec3 skyColor = vec3(0.4, 0.6, 1.0); 
vec3 waterColor = vec3(0.25, 0.4, 0.9);
uniform float renderDist;
uniform float waterLevel;

void main() {
	// ambient
	float ambientStrength = 0.3;
	vec3 ambient = ambientStrength * lightColor;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	float specularStrength = 0.1;
	vec3 viewDir = normalize(camPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
	vec3 specular = specularStrength * spec * lightColor;

	vec4 texColor = texture(ourTexture, TexCoord);
	vec3 lightRes = (ambient + diffuse + specular);

	// fog ?
	float dist = length(camPos - FragPos);
	float fogStart = 32 * (renderDist-2);
	float fogEnd = 32 * renderDist - 16;
	float fogFactor = clamp((dist - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
	// float fogFactor = clamp((dist - (192)) / (240), 0.0, 1.0);

	// looking at highlight
	ivec3 curBlock = ivec3(floor(FragPos + 0.5 - norm * 0.01));
	// ivec3 curBlock = ivec3(FragPos);
	if (curBlock == aimPos && Normal == aimNorm) {
		texColor.rgb = vec3(.6, 0, 0);
		lightRes = vec3(1, 1, 1);
	}

	vec3 res = mix(lightRes * texColor.rgb, skyColor, fogFactor);

    // FragColor = vec4(ourColor, 1.0);
	// FragColor = texture(ourTexture, TexCoord);

	// blue tint on everything for underwater
	if (camPos.y <= waterLevel + 0.3) FragColor = vec4(res * waterColor, texColor.a);
	else FragColor = vec4(res, texColor.a);
}
