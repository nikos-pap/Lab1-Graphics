#shader vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 aNormal;
//layout(location = 1) in vec3 vertexNormal_modelspace;

uniform mat4 u_MVP;
uniform mat4 model;
out vec3 FragPos;
out vec3 Normal;

void main() {
	//lighting calculations
	gl_Position = u_MVP * vec4(position,1.0);
	FragPos = vec4(model * vec4(position,1.0)).xyz;
	Normal = mat3(model)*aNormal;
};

#shader fragment
#version 330 core

layout (location = 0) out vec4 color;

uniform vec3 u_Light;
uniform vec4 u_Color;
in vec3 Normal;
in vec3 FragPos;

void main() {
	// Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(0.6, 0.6, 0.6);
	vec3 ambient = vec3(0.2, 0.2, 0.16);
	//float distance = length(u_Light - vec3(100, 57, 93));//Position_worldspace);

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(u_Light - FragPos);
	float diff = clamp(dot(norm, lightDir), 0.0, 1.0);
	vec3 diffuse = diff * LightColor;


	color = (vec4(ambient,1.0f)+vec4(diffuse,1.0)) *u_Color;
	color.a = u_Color[3];
	//color = vec4(0.0, 0.0, 0.5, 0.1);
};