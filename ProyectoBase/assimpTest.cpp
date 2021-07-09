#include <files.hpp>
#include <model.h>
#include <cam.hpp>

const u32 SCR_WIDTH  = 1280;
const u32 SCR_HEIGHT = 720;
const f32 ASPECT     = (f32)SCR_WIDTH / (f32)SCR_HEIGHT;

Cam* cam;

f32  deltaTime  = 0.0f;
f32  lastFrame  = 0.0f;
bool wireframe  = false;

//ShipPosition
float posX = 0.0f;
float posY = 0.0f;
float posZ = 0.0f;


void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cam->processKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cam->processKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cam->processKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cam->processKeyboard(RIGHT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) {
		posX += 0.5f;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) {
		posX -= 0.5f;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) {
		posZ += 0.5f;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) {
		posZ -= 0.5f;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
		posY += 0.5f;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
		posY -= 0.5f;
	}
}
void key_callback(GLFWwindow*, int key, int, int act, int) {
	wireframe ^= key == GLFW_KEY_E && act == GLFW_PRESS;
}
void mouse_callback(GLFWwindow* window, f64 xpos, f64 ypos) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		cam->movePov(xpos, ypos);
	} else {
		cam->stopPov();
	}
}
void scroll_callback(GLFWwindow*, f64, f64 yoffset) {
	cam->processScroll((f32)yoffset);
}

i32 main() {
	GLFWwindow* window = glutilInit(3, 3, SCR_WIDTH, SCR_HEIGHT, "Cubito");
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	cam = new Cam();

	Files* files = new Files("bin", "resources/textures", "resources/objects");

	//Monito Ship
	Shader* shader = new Shader(files, "shader.vert", "shader.frag");
	Model*  ship = new Model(files, "ship/LPSP_PersonalShip.obj");

	//Metioritos
	int nMeteoros = 300;
	Shader* asteroidShader = new Shader(files, "shader.vert", "shader.frag");
	Model* roca = new Model(files, "rock/rock.obj");
	glm::mat4* metioritos = new glm::mat4[nMeteoros];
	

	f32 offset = 2.5f;
	f32 radius = 5.0f;
	for (u32 i = 0; i < nMeteoros; ++i) {
		glm::mat4 model = glm::mat4(1.0f);
		f32 angle = (f32)i / (f32)nMeteoros * 360.0f;
		f32 displacement = (rand() % (i32)(2 * offset * 100)) / 100.0f - offset;
		f32 x = sin(angle) * radius + displacement;
		displacement = (rand() % (i32)(2 * offset * 100)) / 100.0f - offset;
		f32 y = displacement * 2.0f;
		displacement = (rand() % (i32)(2 * offset * 100)) / 100.0f - offset;
		f32 z = cos(angle) * radius + displacement;
		model = glm::translate(model, { x,y,z });

		f32 scale = (rand() % 10) / 100 + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		f32 rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, { 0.4,0.6,0.8 });

		metioritos[i] = model;
		//std::cout << models[i] << std::endl;
	}



//	std::vector<Model*> meteoros;
//	std::vector<Shader*> shaders;
//
//	for (int i = 0; i < nMeteoros; i++)
//	{
//		std::string fileVert = "";
//		fileVert.append("rock");
//		fileVert.append(std::to_string(i));
//		fileVert.append(".vert");
//		std::string fileFrag = "";
//		fileFrag.append("rock");
//		fileFrag.append(std::to_string(i));
//		fileFrag.append(".frag");
//		Shader* rockShader = new Shader(files, fileVert, fileFrag);
//		shaders.push_back(rockShader);
//	}
//	for (int i = 0; i < nMeteoros; i++)
//	{
//		Model* meteoro = new Model(files, "rock/rock.obj");
//		meteoros.push_back(meteoro);
//	}

	glm::vec3 lightPos   = glm::vec3(1.0f);
	glm::vec3 lightColor = glm::vec3(1.0f);

	glEnable(GL_DEPTH_TEST);

	//Arreglando el tamaño de los asteroides
	float asteroidSize = 5.0f;
	for (u32 i = 0; i < nMeteoros; ++i) {
		metioritos[i] = glm::scale(metioritos[i], glm::vec3(asteroidSize, asteroidSize, asteroidSize));
	}

	while (!glfwWindowShouldClose(window)) {
		f32 currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, wireframe? GL_LINE: GL_FILL);

		shader->use();
		lightPos.x = 2.0f*(cos(currentFrame) - sin(currentFrame));
		lightPos.z = 2.0f*(cos(currentFrame) + sin(currentFrame));
		shader->setVec3("xyz", lightPos);
		shader->setVec3("xyzColor", lightColor);
		shader->setVec3("xyzView", cam->pos);
		glm::mat4 proj = glm::perspective(cam->zoom, ASPECT, 0.1f, 100.0f);
		shader->setMat4("proj", proj);
		shader->setMat4("view", cam->getViewM4());

		float xOffset = 0.0f;
		float yOffset = -1.5f;
		float zOffset = -1.5f;

		//posX = cam->pos.x + xOffset;
		//posY = cam->pos.y + yOffset;
		//posZ = cam->pos.z + zOffset;

		//MONO SHIP
		float shipSize = 0.25f;
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(shipSize, shipSize, shipSize));
		model = translate(model, glm::vec3(posX, posY, posZ));
		shader->setMat4("model", model);
		
		ship->Draw(shader);

		//METEORITOS
//		for (int i = 0; i < nMeteoros; i++)
//		{
//			shaders[i]->use();
//			glm::mat4 modeloRoca = glm::mat4(1.0f);
//			modeloRoca = translate(modeloRoca, glm::vec3(0.0f, 0.0f, 0.0f));
//			shaders[i]->setMat4("model", modeloRoca);
//
//			meteoros[i]->Draw(shaders[i]);
//		}

		asteroidShader->use();
		asteroidShader->setVec3("xyz", lightPos);
		asteroidShader->setVec3("xyzColor", lightColor);
		asteroidShader->setVec3("xyzView", cam->pos);
		asteroidShader->setMat4("proj", proj);
		asteroidShader->setMat4("view", cam->getViewM4());
		
//		std::cout << "-----------------------------" << std::endl;
//
//		std::cout << model[3].x << " | " << model[3].y << " | " << model[3].z << std::endl;
//
//		std::cout << "-----------------------------" << std::endl;

		for (u32 i = 0; i < nMeteoros; ++i) {
			asteroidShader->setMat4("model", metioritos[i]);
			roca->Draw(asteroidShader);

			if (sqrt(pow(metioritos[i][3].x - model[3].x, 2)
					+ pow(metioritos[i][3].y - model[3].y, 2)
					+ pow(metioritos[i][3].z - model[3].z, 2)) <= 0.8f) {
				metioritos[i][3].x = 9999;
				metioritos[i][3].y = 9999;
				metioritos[i][3].z = 9999;
			}
		}


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	delete cam;
	delete shader;
	delete ship;
	delete asteroidShader;
	delete roca;
	delete[] metioritos;

//	for (Shader* var : shaders)
//	{
//		delete var;
//	}
//
//	for (Model* var : meteoros)
//	{
//		delete var;
//	}

	delete asteroidShader;
	delete[] metioritos;
	delete roca;

	return 0;
}

/* vim: set tabstop=2:softtabstop=2:shiftwidth=2:noexpandtab */

