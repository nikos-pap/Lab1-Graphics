﻿#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
// Include GLEW needs to be before any other openGL includes
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "DynamicShapeArray.h"
#include <glm/gtx/string_cast.hpp>
#include <array> 
#include "shader.h"

unsigned int circle_indices2[] = {
		0, 1, 2,
		3, 1, 4,
		5, 1, 6,
		7, 1, 8,
		9, 1, 10,
		11, 1, 12,
		13, 1, 14,
		15, 1, 16,
		17, 1, 18,
		19, 1, 20,
		21, 1, 22,
		23, 1, 24,
		25, 1, 26,
		27, 1, 28,
		29, 1, 30,
		31, 1, 32,
		33, 1, 34,
		35, 1, 36,
		37, 1, 38,
		39, 1, 40,
		41, 1, 42,
		43, 1, 44,
		45, 1, 46,
		47, 1, 48,
		49, 1, 50,
		51, 1, 52,
		53, 1, 54,
		55, 1, 56,
		57, 1, 58,
		59, 1, 60,
		61, 1, 62,
		63, 1, 64,
		65, 1, 66,
		67, 1, 68,
		69, 1, 70,
		71, 1, 72,
};

/*
	Indices for cube triangle points have been numbered in the following way on the 2 faces back and front(+4)
	1 - 3 5 - 7
	| X | | X |
	0 - 2 4 - 6
	back: 0123, front: 4567, left: 0145, right: 2367, bottom: 4062, top: 5173
	to get actual pos though you need to multiply with 3
*/
unsigned int cube_indices[] = {
		4, 5, 6,//front
		5, 6, 7,//front
		0, 1, 2,//back
		1, 2, 3,//back
		0, 1, 4,//left
		1, 4, 5,//left
		2, 3, 6,//right
		3, 6, 7,//right
		5, 1, 7,//top
		1, 7, 3,//top
		4, 0, 6,//bottom
		0, 6, 2//bottom
};

DynamicShapeArray shapeArray;

int main(void) {
	GLFWwindow* window;

	if (!glfwInit()) {
		return -1;
	}

	window = glfwCreateWindow(600, 600, u8"Συγκρουόμενα", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	/* need to do this after I have a valid context */
	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;

	glm::mat4 Projection = glm::ortho(-200.0f, 200.0f, -200.0f, 150.0f, 250.0f, -100.0f);

	glm::mat4 View = glm::lookAt(
		glm::vec3(100.0f, 150.0f, 100.0f), // Camera is at (4,3,3), in World Space
		//glm::vec3(0.0f, 150.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f), // and looks at the origin
		glm::vec3(0.0f, 1.0f, 0.0f)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);

	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
	//MVP = glm::mat4(1.0f);
	// generate a buffer store its id in buffer and bind it
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	//create main cube
	shapeArray.CreateCube(0.0f, 0.0f, 0.0f, 100.0f);
	int num_of_normals;
	glm::vec3* normals = shapeArray.GetCubeNormals(0, &num_of_normals);
	glBufferData(GL_ARRAY_BUFFER, 3 * 8 * sizeof(float), shapeArray.GetShape(0), GL_STATIC_DRAW);
	for (int i = 0; i < num_of_normals; i++) {
		std::cout << glm::to_string(normals[i]) << std::endl;
	}

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	
	// Enable depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// index buffer to tell which positions we need
	unsigned int ibo;
	//glGenBuffers creates the random id for that buffer and stores it in the variable
	glGenBuffers(1, &ibo);
	//bind object buffer to target
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * 2 * 6 * sizeof(unsigned int), cube_indices, GL_STATIC_DRAW);
	//shapeArray.CreateCircle(0.0,0.0f,0.0f,10.0f);
	//glBufferData(GL_ARRAY_BUFFER, 2 * 39 * sizeof(float), shapeArray.GetShape(1), GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*2*6 * sizeof(unsigned int), circle_indices2, GL_STATIC_DRAW);
	Shader shader("Shader.shader");
	glm::vec3 lightPos = glm::vec3(150.0f, 150.0f, 150.0f);
	shader.SetUniformMat4f("u_MVP", MVP);
	shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 0.1f);
	shader.SetUniform3f("u_Light", 150.0f, 150.0f, 150.0f);
	shader.Bind();
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
		shader.Bind();
		/* render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.SetUniformMat4f("u_MVP", MVP);
		//glUniform4f(colorID, 1.0f, 1.0f, 1.0f, 1.0f);
		shader.SetUniform3f("u_Light", 150.0f, 150.0f, 150.0f);
		shader.SetUniform4f("u_Color", 1.0f, 0.0f, 1.0f, 0.3f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
		shader.Unbind();
	}
	//glDeleteProgram(shader);
	glfwTerminate();

	return 0;
}