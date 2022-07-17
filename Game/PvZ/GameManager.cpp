#include "GameManager.h"

#include <thread>
#include <GLFW/glfw3.h>
#include "LocalPlayer.h"
#include "Animator.h"
#include "EZNgine.h"
#include "StaticModel.h"

///IMPORTANT: model.setRotation => ok
///			 dinamic => pas ok, remplacer par getHitbox.setRotationAroundCenter
///TODO: setup materials in models

GameManager::GameManager(glm::mat4 proj) : projection(proj)
{
}

void GameManager::loop(Shader& shader, double timeStamp)
{
	currentTimestamp = timeStamp;
	std::unique_ptr<Camera>& cam = EZNgine::localPlayer->getCamera();
	EZNgine::localPlayer->move(forward, backward, left, right, jump, timeStamp);
	EZNgine::localPlayer->setCameraPosition(EZNgine::localPlayer->getModel()->getPosition());
	EZNgine::localPlayer->getModel()->getHitBox()->setRotationAroundCenter(-cam->getYaw() + cam->getDefaultYaw());

	demoLevel->draw();
	EZNgine::localPlayer->draw(shader);
	cube->draw(shader);
}

void GameManager::loopInstancied(Shader& shader, double timeStamp)
{
}

void GameManager::loopAnimated(Shader& shader, double timeStamp)
{
	animatedModel->draw(shader, *animator, EZNgine::localPlayer->getCamera()->getViewMatrix());
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
	demoLevel = new DemoLevel(engine->shader);

	btDiscreteDynamicsWorld* dynamicsWorld = engine->dynamicsWorld;
	cube = new StaticModel("../../models/cube/cube_2.obj", glm::vec3(-10.0f, 5.0f, 0.0f), HitBoxFactory::AABB);
	animatedModel = new StaticModel("../../models/manequin/manequin_3.fbx", glm::vec3(10.0f, 3.0f, 10.0f), HitBoxFactory::AABB, glm::vec3(0.05f), true);
	EZNgine::localPlayer->getModel()->setPosition(demoLevel->playerSpawnPoint);

	for (auto* rigidBody : EZNgine::localPlayer->getModel()->getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
	}
	for (auto* rigidBody : cube->getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
	}
	for (auto* rigidBody : animatedModel->getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
	}
	for (auto* rigidBody : demoLevel->getModel()->getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
	}

	animator = new Animator(animatedModel->getAnimation(), animatedModel);

	std::thread t([&]()
		{
			float angle = 0;
			while (true) {
				angle += 0.01f;
				animatedModel->getHitBox()->setRotationAroundCenter(angle * 20.0f);
				animator->UpdateAnimation(currentTimestamp * 20.0f);
			}
		});
	t.detach();
}
