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

void GameManager::loop(Shader& shader, double frameTime)
{
	currentTimestamp += frameTime;
	std::unique_ptr<Camera>& cam = EZNgine::localPlayer->getCamera();
	EZNgine::localPlayer->move(forward, backward, left, right, jump, frameTime);
	EZNgine::localPlayer->setCameraPosition(EZNgine::localPlayer->getModel()->getPosition());
	EZNgine::localPlayer->getModel()->getHitBox()->setRotationAroundCenter(-cam->getYaw() + cam->getDefaultYaw());

	checkPlayerFloorColision(frameTime);

	demoLevel->animatePlateforms(currentTimestamp);
	demoLevel->draw();
	EZNgine::localPlayer->draw(shader);
}

void GameManager::loopInstancied(Shader& shader, double frameTime)
{
}

void GameManager::loopAnimated(Shader& shader, double frameTime)
{
	//animatedModel->draw(shader, *animator, EZNgine::localPlayer->getCamera()->getViewMatrix());
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
	if (jump && EZNgine::localPlayer->canJump) {
		EZNgine::localPlayer->lastJumpTimeStamp = currentTimestamp;
	}
}

void GameManager::onInitialized(EZNgine* engine)
{
	demoLevel = new DemoLevel(engine->shader);
	dynamicsWorld = engine->dynamicsWorld;

	btDiscreteDynamicsWorld* dynamicsWorld = engine->dynamicsWorld;
	EZNgine::localPlayer->getModel()->setPosition(demoLevel->playerSpawnPoint);

	for (auto* rigidBody : EZNgine::localPlayer->getModel()->getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
	}
	for (auto* rigidBody : demoLevel->getModel()->getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
	}

	for (auto* model : demoLevel->plateforms) {
		for (auto* rigidBody : model->getRigidBodys()) {
			dynamicsWorld->addRigidBody(rigidBody);
		}
	}

	//animator = new Animator(animatedModel->getAnimation(), animatedModel);

	/*std::thread t([&]()
		{
			float angle = 0;
			while (true) {
				angle += 0.01f;
				animatedModel->getHitBox()->setRotationAroundCenter(angle * 20.0f);
				animator->UpdateAnimation(currentTimestamp * 20.0f);
			}
		});
	t.detach();*/
}

void GameManager::checkPlayerFloorColision(double frameTime)
{
	const auto lp = EZNgine::localPlayer;
	const btVector3 playerPosition = { lp->getPosition().x, lp->getPosition().y, lp->getPosition().z };
	const btVector3 playerPosDownVect = { lp->getPosition().x, lp->getPosition().y - 5.0f, lp->getPosition().z };

	if (!raycastWorld(playerPosition, playerPosDownVect)) //rien en dessous des pieds 
	{
		if (currentTimestamp >= lp->lastJumpTimeStamp + lp->JUMP_TIME) {
			lp->lastJumpTimeStamp = 0;
			lp->isJumping = false;
		}
		lp->canJump = false;
		btVector3 gravity = btVector3(0, GRAVITY_Y, 0);
		for (auto* rigidBody : lp->getModel()->getRigidBodys()) {
			if (rigidBody->getGravity().getY() != GRAVITY_Y) {
				rigidBody->setGravity(gravity);
			}
		}
	}
	else //walking
	{
		lp->canJump = true;
		btVector3 gravity = btVector3(0, 0, 0);
		for (auto* rigidBody : lp->getModel()->getRigidBodys()) {
			if (rigidBody->getGravity().getY() != 0) {
				rigidBody->setGravity(gravity);
			}
		}
	}
}

bool GameManager::raycastWorld(btVector3 Start, btVector3 End) {

	btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
	RayCallback.m_collisionFilterMask = btBroadphaseProxy::AllFilter;
	dynamicsWorld->rayTest(Start, End, RayCallback);
	return RayCallback.hasHit();
}