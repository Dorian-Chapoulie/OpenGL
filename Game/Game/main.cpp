#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <irrKlang/irrKlang.h>
#include "Light.h"
#include "Model.h"
#include <thread>

#include "LocalPlayer.h"

#define WIDTH 800
#define HEIGHT 600
#define FULLSCREEN false

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;
float mouseX, mouseY;

LocalPlayer* localPlayer = nullptr;
irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();

/*
 * model[0][0] => WIDTH
 * model[1][1] => HEIGHT
 * model[3][0] => x
 * model[3][1] => y
 * model[3][2] => z
 * [3][3] => max
 */

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	const static std::unique_ptr<Camera>& cam = localPlayer->getCamera();
	const float cameraSpeed = 10.0f * deltaTime;
	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam->increasePosition(cameraSpeed * cam->getFrontVector());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam->increasePosition(-cameraSpeed * cam->getFrontVector());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam->increasePosition(-glm::normalize(glm::cross(cam->getFrontVector(), cam->getUpVector())) * cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam->increasePosition(glm::normalize(glm::cross(cam->getFrontVector(), cam->getUpVector())) * cameraSpeed);

	const float step = 0.001f;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		
	}
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		
	}
	else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		
	}
	else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
	{
		
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	mouseX = xpos;
	mouseY = ypos;

	const static std::unique_ptr<Camera>& cam = localPlayer->getCamera();

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

void setupShader(Shader& shader)
{
	const glm::mat4 model = glm::mat4(1.0f);
	const glm::mat4 view = localPlayer->getCamera()->getViewMatrix();
	const glm::mat4 projection = glm::perspective(glm::radians(70.0f), static_cast<float>(WIDTH / HEIGHT), 0.1f, 30.0f);

	shader.use();
	shader.setMatrix("model", model);
	shader.setMatrix("view", view);
	shader.setVec3("viewPos", localPlayer->getCamera()->getPosition());
	shader.setMatrix("projection", projection);

	shader.setVec3("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("material.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setValue<float>("material.shininess", 50.0f);
}

void createLights(Shader& shader)
{
	const glm::vec3 downVector = glm::vec3(0.0f, -1.0f, 0.0f);
	const glm::vec3 white = glm::vec3(1.0f);
	const glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 blue = glm::vec3(0.0f, 1.0f, 1.0f);
	const glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);

	auto* light = new Light(&shader, glm::vec3(5.75878, 1.32539, -5.09563), white);

	auto* light2 = new Light(&shader, glm::vec3(-3, 4, 0), downVector, blue, 10.0f);
	auto* light4 = new Light(&shader, glm::vec3(0.5, 4, 0), downVector, red, 10.0f);
	auto* light5 = new Light(&shader, glm::vec3(4, 4, 0), downVector, green, 10.0f);
}

void setupSound()
{
	//SoundEngine->play2D("../../audio/quake/standard/prepare.mp3", false);
	//irrklang::vec3df position(0.0f, 0.8f, -8.0f);
	//SoundEngine->play3D("../../audio/quake/standard/monsterkill.mp3", position);
}

void ray()
{
	const glm::mat4 projection = glm::perspective(glm::radians(70.0f), static_cast<float>(WIDTH / HEIGHT), 0.1f, 30.0f);
	
	float x = (2.0f * mouseX) / WIDTH - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / HEIGHT;
	float z = 1.0f;
	glm::vec3 ray_nds = glm::vec3(x, y, z);
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	glm::vec3 ray_wor = glm::vec3(0.0f);
	ray_wor.x = (glm::inverse(localPlayer->getCamera()->getViewMatrix()) * ray_eye).x;
	ray_wor.y = (glm::inverse(localPlayer->getCamera()->getViewMatrix()) * ray_eye).y;
	ray_wor.z = (glm::inverse(localPlayer->getCamera()->getViewMatrix()) * ray_eye).z;
	
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);

	//std::cout << ray_wor.x << ", " << ray_wor.y << ", " << std::endl;
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

	localPlayer = new LocalPlayer("../../models/crate/Wooden Crate.obj", "../../models/M4a1/M4a1.obj", glm::vec3(0.0f, 0.8f, -8.0f));
	//Model model("../../models/aim_deagle7k/map.obj");
	//Model model("../../models/crate/Wooden Crate.obj");
	//Model model("../../models/M4a1/M4a1.obj");
	Model model("../../models/floor/CobbleStones2.obj", glm::vec3(0.0f, -2.0f, 0.0f));
	//Model model("../../models/crate/Wooden Crate.obj", glm::vec3(0.0f, 0.0f, 0.0f));

	float dx = 0;
	
	Shader shader("./vertex.vert", "./fragment.frag");

	setupShader(shader);
	setupSound();
	createLights(shader);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); POUR AVOIR LES TRAIT DES VERTICES
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		ray();

		//dx += step;

		shader.setVec3("viewPos", localPlayer->getCamera()->getPosition());
		localPlayer->draw(shader);
		shader.setMatrix("view", localPlayer->getCamera()->getViewMatrix());
		model.draw(shader);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}