#include "GameManager.h"
#include "LocalPlayer.h"

void GameManager::processInput(void* w) {
	GLFWwindow* window = (GLFWwindow*)w;
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
}

void GameManager::loop(Shader& shader, double timeStamp)
{
	LocalPlayer* localPlayer = EZNgine::localPlayer;
	std::unique_ptr<Camera>& cam = localPlayer->getCamera();
	localPlayer->move(forward, backward, left, right, jump, timeStamp);
	localPlayer->setCameraPosition(localPlayer->getModel()->getPosition());
	localPlayer->getModel()->getHitBox()->setRotationAroundCenter(-cam->getYaw() + cam->getDefaultYaw());


	shader.setVec3("viewPos", localPlayer->getCamera()->getPosition());
	shader.setMatrix("view", localPlayer->getCamera()->getViewMatrix());

	localPlayer->draw(shader);
	for (Enemy* e : enemys) {
		manageEntity(e, timeStamp, shader);
		for (BasicBullet* b : e->bullets) manageEntity(b, timeStamp, shader);
	}
	map->draw();
};

void GameManager::loopInstancied(Shader& shader, double timeStamp)
{

};

void GameManager::onInitialized(EZNgine* engine)
{
	world = engine->dynamicsWorld;
	map = new LevelKinoDerToten(engine->shader);

	for (int i = 0; i < 1; i++)
	{
		enemys.emplace_back(new Enemy(glm::vec3(1.0f, 6.0f, 0.0f)));
		enemys.back()->onInit(world);
	}

	for (auto* rigidBody : EZNgine::localPlayer->getModel()->getRigidBodys()) {
		world->addRigidBody(rigidBody);
	}
	for (auto* rigidBody : map->getModel()->getRigidBodys()) {
		world->addRigidBody(rigidBody);
	}
}

void GameManager::manageEntity(Entity* e, double timeStamp, Shader& shader)
{
	e->onUpdate(timeStamp, world);
	e->model->draw(shader);
}

