#shader vertex
#version 330 core

layout (location = 0) in vec4 position;
//layout(location = 1) in vec3 vertexNormal_modelspace;

uniform mat4 u_MVP;

void main() {
	gl_Position = u_MVP * position;
};

#shader fragment
#version 330 core

layout (location = 0) out vec4 color;
uniform vec4 u_Color;
uniform vec3 LightPosition_worldspace;

void main() {
	// Light emission properties
	// You probably want to put them as uniforms
	/*vec3 LightColor = vec3(1, 1, 1);
	float LightPower = 50.0f;
	vec3 MaterialAmbientColor = vec3(0.1, 0.1, 0.1);
	float distance = length(LightPosition_worldspace - Position_worldspace);*/

	color = u_Color;
	color.a = u_Color[3];
	//color = vec4(0.0, 0.0, 0.5, 0.1);
};