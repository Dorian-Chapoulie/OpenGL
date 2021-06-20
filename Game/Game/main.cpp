#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <irrKlang/irrKlang.h>
#include "Camera.h"
#include "Model.h"


float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;
std::unique_ptr<Camera>& cam = Camera::getInstance();
irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
float linear = 0.37f, quadra = 0.14f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	const float cameraSpeed = 2.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam->increasePosition(cameraSpeed * cam->getFrontVector());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam->increasePosition(-cameraSpeed * cam->getFrontVector());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam->increasePosition(-glm::normalize(glm::cross(cam->getFrontVector(), cam->getUpVector())) * cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam->increasePosition(glm::normalize(glm::cross(cam->getFrontVector(), cam->getUpVector())) * cameraSpeed);

	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	{
		linear += 0.01f;
	}else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
	{
		linear -= 0.01f;
	}
	else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		quadra += 0.01f;
	}
	else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		quadra -= 0.01f;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	cam->increaseYaw(xoffset);
	cam->increasePitch(yoffset);

	if (cam->getPitch() > 89.0f)
		cam->setPich(89.0f);
	if (cam->getPitch() < -89.0f)
		cam->setPich(-89.0f);

	glm::vec3 direction;
	direction.x = cos(glm::radians(cam->getYaw())) * cos(glm::radians(cam->getPitch()));
	direction.y = sin(glm::radians(cam->getPitch()));
	direction.z = sin(glm::radians(cam->getYaw())) * cos(glm::radians(cam->getPitch()));
	cam->setDirection(direction);
}

int main() {
#pragma region setup
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
#pragma endregion endsetup

	Model backpack("../../models/aim_deagle7k/map.obj");
	//Model backpack("../../models/crate/Wooden Crate.obj");
	Shader ourShader("./vertex.vert", "./fragment.frag");

	//SoundEngine->play2D("../../audio/quake/standard/prepare.mp3", false);
	//irrklang::vec3df position(0.0f, 0.8f, -8.0f);
	//SoundEngine->play3D("../../audio/quake/standard/monsterkill.mp3", position);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = cam->getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	
	
	std::vector<glm::vec3> lightPos;
	lightPos.emplace_back(glm::vec3(5.75878, 1.32539, -5.09563));
	lightPos.emplace_back(glm::vec3(5.77907, 1.36867, 1.26564));
	lightPos.emplace_back(glm::vec3(5.7675, 1.33751, 7.57107));
	lightPos.emplace_back(glm::vec3(-6.1861, 1.34011, 7.65859));
	lightPos.emplace_back(glm::vec3(-6.29876, 1.35056, 1.34397));
	lightPos.emplace_back(glm::vec3(-6.24773, 1.36029, -5.05376));
	lightPos.emplace_back(glm::vec3(0.0675, 3.03751, 0.07107));

	ourShader.use();
	ourShader.setMatrix("model", model);
	ourShader.setMatrix("view", view);
	ourShader.setVec3("viewPos", cam->getPosition());
	ourShader.setMatrix("projection", projection);
	ourShader.setValue<int>("lightNumber", lightPos.size());
	
	ourShader.setVec3("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
	ourShader.setVec3("material.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	ourShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	ourShader.setValue<float>("material.shininess", 50.0f);
	


	for (int i = 0; i < lightPos.size(); i++) {
		ourShader.setVec3("lights[" + std::to_string(i) + "].position", lightPos[i]);
		ourShader.setVec3("lights[" + std::to_string(i) + "].direction", glm::vec3(0.0f, -1.0f, 0.0f));
	
		ourShader.setVec3("lights[" + std::to_string(i) + "].ambient", glm::vec3(0.5f, 0.5f, 0.5f));
		ourShader.setVec3("lights[" + std::to_string(i) + "].diffuse", glm::vec3(1.0f));//émet
		ourShader.setVec3("lights[" + std::to_string(i) + "].specular", glm::vec3(1.0f, 1.0f, 1.0f));//reflet

		if (i == 6)
		{
			ourShader.setValue<float>("lights[" + std::to_string(i) + "].linear", 0.f);
			ourShader.setValue<float>("lights[" + std::to_string(i) + "].quadratic", 0.f);
		}
		else {
			ourShader.setValue<float>("lights[" + std::to_string(i) + "].linear", linear);
			ourShader.setValue<float>("lights[" + std::to_string(i) + "].quadratic", quadra);
		}
		ourShader.setValue<float>("lights[" + std::to_string(i) + "].constant", 1.0f);
		
		ourShader.setValue<float>("lights[" + std::to_string(i) + "].cutOff", glm::cos(glm::radians(50.0f))); //radius
		ourShader.setValue<bool>("lights[" + std::to_string(i) + "].isDirectional",  i != 6);
	}


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); POUR AVOIR LES TRAIT DES VERTICES
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//std::cout << cam->getPosition().x << " ," << cam->getPosition().y << ", " << cam->getPosition().z << std::endl;
		//std::cout << cam->getFrontVector().x << " ," << cam->getFrontVector().y << ", " << cam->getFrontVector().z << std::endl;
		//std::cout << linear << " " << quadra << std::endl;
		
		ourShader.setMatrix("view", cam->getViewMatrix());
		ourShader.setVec3("viewPos", cam->getPosition());
		backpack.draw(ourShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}