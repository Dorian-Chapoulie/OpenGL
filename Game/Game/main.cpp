#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <irrKlang/irrKlang.h>
#include "Light.h"
#include "Model.h"
#include <thread>
#include <Bullet3/btBulletCollisionCommon.h>
#include <Bullet3/btBulletDynamicsCommon.h>
#include <Bullet3/LinearMath/btIDebugDraw.h>
#include "GLDebugDrawer.h"
#include "LocalPlayer.h"
#include "OpenGLine.h"
#include "SkyBox.h"
#include "Animation.h"
#include "Animator.h"
#include "DefaultLoader.h"
#include "DynamicModel.h"
#include "SkeletalLoader.h"
#include "StaticModel.h"

#define WIDTH 800
#define HEIGHT 600
#define FULLSCREEN false
//TODO: change this value 
#define DRAW_DISTANCE 500.0f
#define FOV 70.0f

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;
bool firstMouse = true;
float mouseX, mouseY;
float forceX = 0.0f, forceY = 0.0f, forceZ = 0.0f;
bool forward = false;
bool backward = false;
bool left = false;
bool right = false;
bool jump = false;

LocalPlayer* localPlayer = nullptr;
irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
//TODO: desctructeur model & mesh
//TODO: change model via model as param and not path
//TODO: Static object should not have access to setWorldTransform
/*
 *
 * TODO: DESTRUCTEUR glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteProgram(shaderProgram);
			/*const glm::mat4 mtx_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
	const auto cameraPosition = glm::inverse(camera->getViewMatrix()) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	const glm::vec3 positionTmp = glm::vec3(cameraPosition.x, cameraPosition.y - offsetCameraY, cameraPosition.z);

 */

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		forward = true;
		backward = false;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		backward = true;
		forward = false;
	}
	else {
		forward = false;
		backward = false;
	}


	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		left = true;
		right = false;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		right = true;
		left = false;
	}
	else
	{
		right = false;
		left = false;
	}

	jump = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
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

void scroll_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (ypos == 1)
	{
		localPlayer->decreaseCameraYOffset();
	}
	else
	{
		localPlayer->increaseCameraYOffset();
	}
}

void setupShader(Shader& shader, const glm::mat4& projection)
{
	const glm::mat4 model = glm::mat4(1.0f);
	const glm::mat4 view = localPlayer->getCamera()->getViewMatrix();

	shader.use();
	shader.setMatrix("model", model);
	shader.setMatrix("view", view);
	shader.setVec3("viewPos", localPlayer->getCamera()->getPosition());
	shader.setMatrix("projection", projection);

	shader.setVec3("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setValue<float>("material.shininess", 100.0f);
}

void setupSkyBoxShader(Shader& shader, const glm::mat4& projection)
{
	const glm::mat4 view = localPlayer->getCamera()->getViewMatrix();
	shader.setValue<int>("skybox", 0);
	shader.setMatrix("view", view);
	shader.setMatrix("projection", projection);
}

void createLights(Shader& shader, std::vector<Light*>& lights)
{
	const glm::vec3 downVector = glm::vec3(0.0f, -1.0f, 0.0f);
	const glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
	const glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 blue = glm::vec3(0.0f, 1.0f, 1.0f);
	const glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);

	auto* light = new Light(&shader, glm::vec3(0.f, 5.0f, 0.f), white);
	lights.emplace_back(light);
	//auto* light2 = new Light(&shader, glm::vec3(0, 10, 0), downVector, red, 100.0f);
	//auto* light4 = new Light(&shader, glm::vec3(0.5, 8, 0), downVector, red, 100.0f);
	//auto* light5 = new Light(&shader, glm::vec3(4, 8, 0), downVector, green, 100.0f);
}

void setupSound()
{
	//SoundEngine->play2D("../../audio/quake/standard/prepare.mp3", false);
	//irrklang::vec3df position(0.0f, 0.8f, -8.0f);
	//SoundEngine->play3D("../../audio/quake/standard/monsterkill.mp3", position);
}

void promptFps(int& nbFrames, double& lastTime)
{
	const double currentTime = glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 1.0) {
		std::cout << nbFrames << " FPS\n";
		nbFrames = 0;
		lastTime += 1.0;
	}
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
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPos(window, WIDTH / 2.0f, HEIGHT / 2.0f);
#pragma endregion endsetup

#pragma region physics
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	// The world.
	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0.0f, -9.0f, 0.0f));
	GLDebugDrawer* debugDraw = new GLDebugDrawer();
	debugDraw->DBG_DrawWireframe;
	debugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	dynamicsWorld->setDebugDrawer(debugDraw);
#pragma endregion physics

	std::vector<Light*> lights;
	//../../models/floor_2/floor.obj
	//StaticModel model("../../models/css/css.dae", glm::vec3(0.0f, 0.0f, 0.0f), HitBoxFactory::AABB_MULTIPLE, glm::vec3(5.0f));
	StaticModel model("../../models/floor_2/floor.obj", glm::vec3(0.0f, -5.0f, 0.0f), HitBoxFactory::AABB_MULTIPLE, glm::vec3(1.0f));
	StaticModel model3("../../models/ramp/ramp.obj", glm::vec3(0.0f, -5.0f, -5.0f), HitBoxFactory::TRIANGLE, glm::vec3(0.25f));
	//../../models/manequin/manequin_3.fbx
	localPlayer = new LocalPlayer("../../models/cube/cube.obj", glm::vec3(0, 0, 0));

	std::vector<DynamicModel*> dynamicModels;
	for (int i = 0; i < 0; i++)
	{
		dynamicModels.emplace_back(
			new DynamicModel("../../models/cube/cube.obj", glm::vec3(5, 0, 0), 1.0f, HitBoxFactory::AABB)
		);
	}


	Shader shader("./vertex.vert", "./fragment.frag");
	Shader skyboxShader("./skybox.vert", "./skybox.frag");
	Shader animationShader("./animation.vert", "./animation.frag");
	SkyBox skybox("../../textures/skybox");

	glm::mat4 projection = glm::perspective(glm::radians(FOV), float(WIDTH / HEIGHT), 0.1f, DRAW_DISTANCE);

	setupShader(animationShader, projection);
	setupShader(shader, projection);
	setupSkyBoxShader(skyboxShader, projection);
	setupSound();
	createLights(shader, lights);

	btVector3 g = btVector3(0, 0, 0);
	for (auto* rigidBody : localPlayer->getModel()->getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
		rigidBody->setGravity(g);
	}
	for (auto* rigidBody : model.getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
	}
	for (DynamicModel* m : dynamicModels)
	{
		for (auto* rigidBody : m->getRigidBodys()) {
			dynamicsWorld->addRigidBody(rigidBody);
		}
	}
	for (auto* rigidBody : model3.getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
	}

	//Animator animator(model3.getAnimation(), &model3);

	/*std::thread t([&]()
		{
			int dx = 1;
			float i = 0;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			while (true) {
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
				model3.setPosition(glm::vec3(0, 3, -5 + i));
				model3.setRotation(glm::vec3(0, 1, 0), i += 0.1f);
				animator.UpdateAnimation(deltaTime);
			}
		});*/

	const static std::unique_ptr<Camera>& cam = localPlayer->getCamera();
	float timeStep = 1 / 10.0f;
	int nbFrames = 0;
	double lastTime = glfwGetTime();
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //POUR AVOIR LES TRAIT DES VERTICES
	bool test = true;
	SkeletalLoader::cache.clear();
	DefaultLoader::cache.clear();
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (Light* l : lights)
		{
			l->draw(localPlayer->getCamera()->getViewMatrix(), projection);
		}

		dynamicsWorld->stepSimulation(timeStep, 10);
		dynamicsWorld->debugDrawWorld();

		const float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		promptFps(nbFrames, lastTime);

		localPlayer->move(forward, backward, left, right, jump, deltaTime);
		localPlayer->setCameraPosition(localPlayer->getModel()->getPosition());
		localPlayer->getModel()->getHitBox()->setRotationAroundCenter(-cam->getYaw() + cam->getDefaultYaw());

		//model3.draw(animationShader, animator, localPlayer->getCamera()->getViewMatrix());


		shader.use();
		//viewpos inutile
		shader.setVec3("viewPos", localPlayer->getCamera()->getPosition());
		shader.setMatrix("view", localPlayer->getCamera()->getViewMatrix());

		localPlayer->draw(shader);
		model.draw(shader);
		//model3.draw(shader);

		for (DynamicModel* m : dynamicModels)
		{
			m->draw(shader);
		}

#pragma region SKYBOX
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		glm::mat4 view = glm::mat4(glm::mat3(localPlayer->getCamera()->getViewMatrix()));
		skyboxShader.setMatrix("view", view);
		skyboxShader.setMatrix("projection", projection);
		skybox.draw();
#pragma endregion 
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}