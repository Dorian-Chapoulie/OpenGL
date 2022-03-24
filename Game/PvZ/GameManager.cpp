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

	checkCollisions();

	std::cout << playerEntity->health << std::endl;

	localPlayer->draw(shader);
	for (Enemy* e : enemys) {
		manageEntity(e, timeStamp, shader);
		for (int i = 0; i < e->bullets.size(); i++) {
			BasicBullet* b = e->bullets[i];
			if (b->state == Entity::ENTITY_STATE::DEAD)
			{
				e->removeBullet(i);
				deleteEntity(b);
			}
			else {
				manageEntity(b, timeStamp, shader);
				checkLifeTime(b);
			}
		}
		checkLifeTime(e);
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
	playerEntity = new PlayerEntity(engine->localPlayer);

	triggerTest = new Trigger([&](void* data)
		{
			std::cout << "Trigger collided with localplayer" << std::endl;
			playerEntity->health = 0.0f;
		}, { 20, 5, 5 });


	triggerTest->onInit(world);
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

void GameManager::checkCollisions()
{
	const int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++) {
		const btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		Entity* entityA = static_cast<Entity*>(obA->getUserPointer());
		Entity* entityB = static_cast<Entity*>(obB->getUserPointer());

		if (entityA && entityB) entityA->onCollide(entityB);
	}
}

void GameManager::checkLifeTime(Entity* e)
{
	if (e->getLifeTime() >= e->getMaxLifeTime()) {
		e->state = Entity::ENTITY_STATE::DEAD;
	}
}

void GameManager::deleteEntity(Entity* e)
{
	for (btRigidBody* r : e->model->getRigidBodys()) {
		world->removeCollisionObject(r);
	}
	delete e;
}

