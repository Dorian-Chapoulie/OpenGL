#include "GameManager.h"

#include <thread>
#include <GLFW/glfw3.h>
#include "LocalPlayer.h"
#include "Animator.h"
#include "EZNgine.h"
#include "StaticModel.h"

StaticModel* model = nullptr;
StaticModel* model3 = nullptr;
Animator* animator = nullptr;
float deltaTest = 0.0f;

GameManager::GameManager(glm::mat4 proj) : projection(proj)
{
}

void GameManager::loop(Shader& shader, double timeStamp)
{
	deltaTest = timeStamp;
	std::unique_ptr<Camera>& cam = EZNgine::localPlayer->getCamera();
	EZNgine::localPlayer->move(forward, backward, left, right, jump, timeStamp);
	EZNgine::localPlayer->setCameraPosition(EZNgine::localPlayer->getModel()->getPosition());
	EZNgine::localPlayer->getModel()->getHitBox()->setRotationAroundCenter(-cam->getYaw() + cam->getDefaultYaw());

	EZNgine::localPlayer->draw(shader);
	model->draw(shader);
}

void GameManager::loopInstancied(Shader& shader, double timeStamp)
{
}

void GameManager::loopAnimated(Shader& shader, double timeStamp)
{
	model3->draw(shader, *animator, EZNgine::localPlayer->getCamera()->getViewMatrix());
}

void GameManager::processInput(void* w) {
	GLFWwindow* window = (GLFWwindow*)w;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && mouseEnabled) {
		glfwSetWindowShouldClose(window, true);
	}

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

	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		mouseEnabled = !mouseEnabled;
	}

	jump = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
}

void GameManager::onInitialized(EZNgine* engine)
{
	btDiscreteDynamicsWorld* dynamicsWorld = engine->dynamicsWorld;
	model = new StaticModel("../../models/floor_2/floor.obj", glm::vec3(0.0f, -5.0f, 0.0f), HitBoxFactory::AABB, glm::vec3(1.0f), false);
	model3 = new StaticModel("../../models/manequin/manequin_3.fbx", glm::vec3(0.0f, 5.0f, -5.0f), HitBoxFactory::AABB, glm::vec3(0.05f), true);
	EZNgine::localPlayer->getModel()->setPosition({ 0.0f, 0.0f, 0.0f });

	for (auto* rigidBody : EZNgine::localPlayer->getModel()->getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
	}
	for (auto* rigidBody : model3->getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
	}
	for (auto* rigidBody : model->getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
	}

	animator = new Animator(model3->getAnimation(), model3);

	std::thread t([&]()
		{
			int dx = 1;
			float i = 0;
			//model3.setPosition(glm::vec3(0, 10, 0));
			//model3.setRotation(glm::vec3(0, 1, 0), 90.0f);

			while (true) {
				model3->setRotation(glm::vec3(0, 1, 0), i += 0.01f);
				animator->UpdateAnimation(deltaTest * 20.0f);
			}
		});
	t.detach();
}

/*
void GameManager::setWindow(GLFWwindow* window)
{
	this->window = window;
}*/
