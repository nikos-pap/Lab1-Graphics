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
	
	// Deprecated, moved to OpenGLRenderer
	unsigned int textureID;
	
	renderer = new OpenGLRenderer();
	if (renderer->init(1000, 1000) != 1) return -1; // TODO: Update to error codes
	window = renderer->getWindow();
	if (!window) return -1;
	shapeArray->setRenderer(renderer);
	
	glm::mat4 Projection = glm::perspective(glm::radians(40.0f), 1.0f, 0.001f, 1000.0f);

	// Model Matrix
	glm::mat4 Model = glm::mat4(1.0f);

	//ModelViewProjection Matrix
	glm::mat4 MVP;// Projection * View * Model;

	shapeArray->InitFactoryPrototypes();


	//Create the first 2 shapes
	shapeArray->CreateShape(0.0f, 0.0f, 0.0f, 100.0f, T_CUBE);
	shapeArray->SetRandomColor(0, 0.5f);//give random color to cube
	shapeArray->CreateShape(35.0f, 35.0f, 35.0f, 30.0f, T_SPHERE);
	shapeArray->SetColor(1, 1.0f, 1.0f, 1.0f, 1.0f);

	//Initialize Shader
	Shader shader("Shader.shader");
	shader.Bind();
	shader.SetUniformMat4f("model", Model);
	unsigned int ib_size;
	uint32_t shapeArrSize;
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
		shapeArrSize = shapeArray->GetSize();
		renderer->clear();

		x += l;
		if (x >= 150.0f || x <= 0.0f) {
			l = (-1.0f) * l;
		}
		MVP = Projection * camera->getView() * Model;

		// This can be a compute shader, then batch draw (utilize instancing and ssbos).
		// Example:
		// moveShader.bind()
		// glDispatchCompute(shapeArrSize, 1, 1);
		for (int i = shapeArrSize - 1; i >= 0; i--) {
			float* color = shapeArray->GetColor(i);
			ib_size = shapeArray->GetIndexPointerSize(i);
			shapeArray->BindShape(i);
			if (i > 1) {
				shapeArray->Move(i);
			}
			Model = shapeArray->GetModel(i);
			MVP = Projection * camera->getView() * Model;
			// update this to SSBOs
			shader.SetUniformMat4f("u_MVP", MVP);
			shader.SetUniform4f("u_Color", color);
			shader.SetUniformMat4f("model", Model);
			shader.SetUniform3f("u_Light", 150.0f, x, 150.0f);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			shader.SetUniform3f("u_vPos", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);

			if (i == 1 && tex) {

				shader.SetUniform1i("isTexture", 2);
			}
			else {
				shader.SetUniform1i("isTexture", 1);
			}
			renderer->drawElements(ib_size);
		}

		renderer->endFrame();
	}
	glfwTerminate();

	return 0;
}
