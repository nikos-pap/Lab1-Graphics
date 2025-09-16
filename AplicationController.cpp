#include "ApplicationController.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION   
#include "stb_image.h"


ApplicationController::ApplicationController() {
	window = nullptr;
	camera = new CameraController();
	shapeArray = new DynamicShapeArray();
	inputController = new InputController(camera, shapeArray);
}

//loads texture from file
unsigned int ApplicationController::loadTexture()
{
	unsigned int texture;
	glGenTextures(1, &texture);

	//generating cube map
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	//Define all 6 faces
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load("textures/texture.jpg", &width, &height, &nrChannels, STBI_rgb_alpha);
	if (data)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);



	stbi_image_free(data);

	return texture;
}

int ApplicationController::start() {
	unsigned int textureID;

	if (!glfwInit()) {
		return -1;
	}


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1000, 1000, u8"Συγκρουόμενα", NULL, NULL);
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
	glm::mat4 Projection = glm::perspective(glm::radians(40.0f), 1.0f, 0.1f, 1000.0f);



	// Model Matrix
	glm::mat4 Model = glm::mat4(1.0f);

	//ModelViewProjection Matrix
	glm::mat4 MVP;// Projection * View * Model;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glEnable(GL_NORMALIZE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//for the textures
	glDisable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	shapeArray->InitFactoryPrototypes();

	//Create the first 2 shapes
	shapeArray->CreateShape(0.0f, 0.0f, 0.0f, 100.0f, T_CUBE);
	shapeArray->SetRandomColor(0, 0.5f);//give random color to cube
	shapeArray->CreateShape(35.0f, 35.0f, 35.0f, 30.0f, T_SPHERE);
	shapeArray->SetColor(1, 1.0f, 1.0f, 1.0f, 1.0f);

	//Initialize Shader
	Shader shader("Shader.shader");
	shader.SetUniformMat4f("model", Model);
	unsigned int ib_size;
	int shapeArrSize;
	float x = 1.0f;
	float l = 1.0f;
	textureID = loadTexture();
#ifdef _WIN32
	mciSendString("open \"Elevator Music.mp3\" type mpegvideo alias mp3", NULL, 0, NULL);
	mciSendString("play mp3 repeat", NULL, 0, NULL);
#endif

	while (inputController->parseInputs(window) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
#ifdef _WIN32
		if (soundsEnabled)
			mciSendString("resume mp3 ", NULL, 0, NULL);
		else
			mciSendString("pause mp3 ", NULL, 0, NULL);
#endif
		shader.Bind();
		shapeArrSize = shapeArray->GetSize();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		x += l;
		if (x == 150.0f || x == 0.0f) {
			l = (-1.0f) * l;
			//std::cout << "change" << std::endl;
		}
		MVP = Projection * camera->getView() * Model;

		for (int i = shapeArrSize - 1; i >= 0; i--) {
			float* color = shapeArray->GetColor(i);
			ib_size = shapeArray->GetIndexPointerSize(i);
			shapeArray->BindShape(i);
			if (i > 1) {
				shapeArray->Move(i);
			}
			Model = shapeArray->GetModel(i);
			MVP = Projection * camera->getView() * Model;
			shader.SetUniformMat4f("u_MVP", MVP);
			shader.SetUniform4f("u_Color", color);
			shader.SetUniformMat4f("model", Model);
			shader.SetUniform3f("u_Light", 150.0f, x, 150.0f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			shader.SetUniform3f("u_vPos", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);

			if (i == 1 && tex) {

				shader.SetUniform1i("isTexture", 2);
				glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
				glDrawElements(GL_TRIANGLES, ib_size, GL_UNSIGNED_INT, nullptr);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			}
			else {
				shader.SetUniform1i("isTexture", 1);
				glDrawElements(GL_TRIANGLES, ib_size, GL_UNSIGNED_INT, nullptr);
			}
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
		shader.Unbind();
	}
	glfwTerminate();

	return 0;
}
