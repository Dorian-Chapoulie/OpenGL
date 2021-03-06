#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <irrKlang/ik_ISoundEngine.h>

#include "BaseApplication.h"
#include "Light.h"
#include "LocalPlayer.h"
#include "OpenGLine.h"
#include "SkyBox.h"

class EZNgine
{
public:
	EZNgine(const glm::mat4 projection, BaseApplication* base_application);

	static LocalPlayer* localPlayer;
	Shader shader;

	static irrklang::ISoundEngine* soundEngine;
	btDiscreteDynamicsWorld* dynamicsWorld;
	GLFWwindow* window = nullptr;
	void loop();

	//static int WIDTH, HEIGHT;
	//static bool FULLSCREEN;

private:
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xpos, double ypos);

	void setupShader(Shader& shader);
	void setupSkyBoxShader();
	void setupSound();
	void setupOpenGl();
	void setupBulletPhysics();
	void setupImGui();
	void setupLight();
	void promptFps(int& nbFrames, double& lastTime);

	void init();


	Shader instancedShader;
	Shader skyboxShader;
	Shader animationShader;
	SkyBox* skybox;

	glm::mat4 projection;

	Light* defaultLight;

	static BaseApplication* base_application;
};

