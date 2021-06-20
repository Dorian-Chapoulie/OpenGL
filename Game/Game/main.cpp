#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <irrKlang/irrKlang.h>
#include "Camera.h"
#include "Light.h"
#include "Model.h"
#include <thread>

#define WIDTH 800
#define HEIGHT 600
#define FULLSCREEN false

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
	glfwWindowHint(GLFW_REFRESH_RATE, 144);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", FULLSCREEN ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
#pragma endregion endsetup

	Model box("../../models/aim_deagle7k/map.obj");
	//Model box("../../models/crate/Wooden Crate.obj");
	Shader ourShader("./vertex.vert", "./fragment.frag");

	//SoundEngine->play2D("../../audio/quake/standard/prepare.mp3", false);
	//irrklang::vec3df position(0.0f, 0.8f, -8.0f);
	//SoundEngine->play3D("../../audio/quake/standard/monsterkill.mp3", position);

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = cam->getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(70.0f), static_cast<float>(WIDTH / HEIGHT), 0.1f, 30.0f);

	ourShader.use();
	ourShader.setMatrix("model", model);
	ourShader.setMatrix("view", view);
	ourShader.setVec3("viewPos", cam->getPosition());
	ourShader.setMatrix("projection", projection);

	ourShader.setVec3("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
	ourShader.setVec3("material.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	ourShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	ourShader.setValue<float>("material.shininess", 50.0f);
	
	glm::vec3 downVector = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 white = glm::vec3(1.0f);
	glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 blue = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 paleBlue = glm::vec3(0.58f, 0.87f, 0.96f);
	
	auto* light = new Light(&ourShader, glm::vec3(5.75878, 1.32539, -5.09563), paleBlue);
	auto* light2 = new Light(&ourShader, glm::vec3(5.77907, 1.36867, 1.26564), downVector, white, 50.0f);
	auto* light3 = new Light(&ourShader, glm::vec3(5.7675, 1.33751, 7.57107), downVector, red, 4.0f);
	auto* light4 = new Light(&ourShader, glm::vec3(-6.1861, 1.34011, 7.65859), downVector, blue,10.0f);
	auto* light5 = new Light(&ourShader, glm::vec3(-6.29876, 1.35056, 1.34397), downVector, white,10.0f);
	auto* light6 = new Light(&ourShader, glm::vec3(-6.24773, 1.36029, -5.05376), downVector, red,10.0f);
	auto* light7 = new Light(&ourShader, glm::vec3(0.0675, 3.03751, 0.07107), downVector, blue,37.0f);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); POUR AVOIR LES TRAIT DES VERTICES
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//light->initShader();

		//std::cout << cam->getPosition().x << " ," << cam->getPosition().y << ", " << cam->getPosition().z << std::endl;
		//std::cout << cam->getFrontVector().x << " ," << cam->getFrontVector().y << ", " << cam->getFrontVector().z << std::endl;
		//std::cout << linear << " " << quadra << std::endl;

		const auto cam_world = inverse(cam->getViewMatrix()) * glm::vec4(0, 0, 0, 1);

		std::cout << cam_world.x << ", " << cam_world.y << ", " << cam_world.z << std::endl;

		ourShader.setMatrix("view", cam->getViewMatrix());
		ourShader.setVec3("viewPos", cam->getPosition());
		//backpack.draw(ourShader);
		box.draw(ourShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}