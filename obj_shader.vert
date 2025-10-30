#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 aNormal;

layout (std140, binding = 0) uniform Matrices {
	mat4 u_MVP;
	mat4 model;
	mat3 normalModel;
};

layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec3 TexCoords;

void main() {
	//lighting calculations
	gl_Position = u_MVP * vec4(position,1.0);
	FragPos = vec4(model * vec4(position,1.0)).xyz;
	//Normal = transpose(inverse(mat3(model))) * normalize(aNormal);
	Normal = normalModel * normalize(aNormal);
	
	//pass texcoords to fragment shader
	TexCoords = position;
};
