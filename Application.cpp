#include "DynamicShapeArray.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

DynamicShapeArray shapeArray;
//camera 
glm::vec3 cameraPos(200.0f, 200.0f, 200.0f);
glm::vec3 cameraFront(-1.0f, -1.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
glm::mat4 View = glm::lookAt(
	cameraPos, // Camera is at (100,150,100), in World Space
	//glm::vec3(0.0f, 150.0f, 0.0f),
	(cameraPos + cameraFront), // and looks at the origin
	cameraUp  // Head is up (set to 0,-1,0 to look upside-down)
);

//speeds
float cameraSpeed = 20.0f / GLOBAL_SPEED;
float yawSpeed = 1.0f / GLOBAL_SPEED;

void processCameraMovement(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos += cameraUp * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		cameraPos -= cameraUp * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		cameraFront += yawSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		cameraFront -= yawSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		cameraFront -= glm::normalize(glm::cross(cameraFront, cameraUp)) * (yawSpeed / 2);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		cameraFront += glm::normalize(glm::cross(cameraFront, cameraUp)) * (yawSpeed / 2);
	View = glm::lookAt(
		cameraPos, // Camera is at (100,150,100), in World Space
		//glm::vec3(0.0f, 150.0f, 0.0f),
		(cameraPos + cameraFront), // and looks at the origin
		cameraUp  // Head is up (set to 0,-1,0 to look upside-down)
	);
}


int main(void) {
	GLFWwindow* window;

	if (!glfwInit()) {
		return -1;
	}


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(600, 600, u8"Συγκρουόμενα Φούντο γαμιέσαι", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}


	glfwMakeContextCurrent(window);

	/* need to do this after I have a valid context */
	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	std::cout << glGetString(GL_VERSION) << std::endl;
	glm::mat4 Projection = glm::perspective(glm::radians(40.0f),1.0f,0.001f,1000.0f); //ortho coords: -200.0f, 200.0f, -200.0f, 150.0f, 250.0f, 0.0f



	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);

	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP;// = Projection * View * Model; // Remember, matrix multiplication is the other way around
	//MVP = glm::mat4(1.0f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glEnable(GL_NORMALIZE);
	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);

	/*look here I replaced the individual shape functions with a single function*/
	/*shapeArray.CreateShape(0.0f, 0.0f, 0.0f, 100.0f, T_SPHERE);
	shapeArray.SetColor(0, 1.0f, 0.0f, 0.1f, 0.5f);
	shapeArray.CreateShape(0.0f, 100.0f, 100.0f, 10.0f, T_CYLINDER);
	shapeArray.SetColor(1, 0.0f, 0.7f, 0.5f, 1.0f);*/
	shapeArray.CreateShape(0.0f, 0.0f, 0.0f, 100.0f, T_CUBE);
	shapeArray.SetColor(0, 0.0f, 0.0f, 1.0f, 0.5f);

	Shader shader("Shader.shader");
	//glm::vec3 lightPos = glm::vec3(150.0f, 150.0f, 150.0f);//not used consider removing
	float light[3] = { 150.0f,-100000.0f,50.0f};
	shader.SetUniform3f("u_Light", light);
	shader.SetUniformMat4f("model", Model);
	unsigned int ib_size;
	int shapeArrSize;
	bool spaceChecker = true;


	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {

		shader.Bind();
		shapeArrSize = shapeArray.GetSize();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)&&spaceChecker) {
			spaceChecker = false;
			shapeArray.CreateRandomShape();
		}
		else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
			spaceChecker = true;
		}
		
		processCameraMovement(window);
		MVP = Projection * View * Model;

		for (int i = 0; i < shapeArrSize; i++) {
			float* color = shapeArray.GetColor(i);
			ib_size = shapeArray.GetIndexPointerSize(i);
			shapeArray.BindShape(i);
			//std::cout << "3" << std::endl;
			shader.SetUniformMat4f("u_MVP", MVP);
			shader.SetUniform4f("u_Color",color);
			shader.SetUniformMat4f("model", View);
			//shader.SetUniform3f("u_Light", 150.0f, 150.0f, 150.0f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			/*if (i == 1) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}*/
			
			glDrawElements(GL_TRIANGLES, ib_size, GL_UNSIGNED_INT, nullptr);
			//glBindVertexArray(0); //it works without this*/
		}

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