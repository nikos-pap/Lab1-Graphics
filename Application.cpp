#include "DynamicShapeArray.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

DynamicShapeArray shapeArray;

//not used anymore consider removing
void createBuffer(int shape_index) {
	unsigned int buffer_id;
	float* shape = shapeArray.GetShape(shape_index);
	/*for (int i = 0; i < 108;i+=3) {
		std::cout << "x: " << shape[i] << ", y: " << shape[i+1] << ", z: " << shape[i+2] << std::endl;

	}*/
	int shape_size = shapeArray.GetSize(shape_index);
	int index_pointer_size = shapeArray.GetIndexPointerSize(shape_index);
	unsigned int* index_array = shapeArray.GetIndexPointer(shape_index);

	unsigned int vao;
	//create and bind the vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//create a buffer to keep out positions
	glGenBuffers(1, &buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
	glBufferData(GL_ARRAY_BUFFER, shape_size * sizeof(float), shape, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	//create a buffer for the indexes
	unsigned int ibo;
	//glGenBuffers creates the random id for that buffer and stores it in the variable
	glGenBuffers(1, &ibo);
	//bind object buffer to target
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_pointer_size * sizeof(unsigned int), index_array, GL_STATIC_DRAW);

	//keep the three buffers in the shape
	shapeArray.SetVAOID(shape_index, vao);
	shapeArray.SetBufferID(shape_index, buffer_id);
	shapeArray.SetIBOID(shape_index, ibo);
	std::cout << "buffer created id's are:" << vao << ", " << buffer_id << ", " << ibo << std::endl;
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

	glm::mat4 View = glm::lookAt(
		glm::vec3(200.0f, 200.0f, 200.0f), // Camera is at (100,150,100), in World Space
		//glm::vec3(0.0f, 150.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f), // and looks at the origin
		glm::vec3(0.0f, 1.0f, 0.0f)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);

	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
	//MVP = glm::mat4(1.0f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*look here I replaced the individual shape functions with a single function*/
	/*shapeArray.CreateShape(0.0f, 0.0f, 0.0f, 100.0f, T_SPHERE);
	shapeArray.SetColor(0, 1.0f, 0.0f, 0.1f, 0.5f);
	shapeArray.CreateShape(100.0f, 100.0f, 100.0f, 40.0f, T_CYLINDER);
	shapeArray.SetColor(1, 0.0f, 0.7f, 0.5f, 1.0f);*/
	shapeArray.CreateShape(0.0f, 0.0f, 0.0f, 100.0f, T_CUBE);
	shapeArray.SetColor(0, 0.0f, 0.0f, 1.0f, 0.5f);

	Shader shader("Shader.shader");
	//glm::vec3 lightPos = glm::vec3(150.0f, 150.0f, 150.0f);//not used consider removing
	float light[3] = { 150.0f,0.0f,50.0f};
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
		for (int i = 0; i < shapeArrSize; i++) {
			float* color = shapeArray.GetColor(i);
			ib_size = shapeArray.GetIndexPointerSize(i);
			shapeArray.BindShape(i);
			//std::cout << "3" << std::endl;
			shader.SetUniformMat4f("u_MVP", MVP);
			shader.SetUniform4f("u_Color",color);
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