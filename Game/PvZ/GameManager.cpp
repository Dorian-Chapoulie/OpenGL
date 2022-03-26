#include "GameManager.h"
#include "LocalPlayer.h"
#include <imgui/imgui.h>

GameManager::GameManager(glm::mat4 proj)
{
	this->projection = proj;
}

void GameManager::processInput(void* w) {
	GLFWwindow* window = (GLFWwindow*)w;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && mouseEnabled) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		playerEntity->shoot(world);
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

void GameManager::loop(Shader& shader, double timeStamp)
{
	LocalPlayer* localPlayer = EZNgine::localPlayer;
	std::unique_ptr<Camera>& cam = localPlayer->getCamera();

	localPlayer->move(forward, backward, left, right, jump, timeStamp);
	localPlayer->setCameraPosition(localPlayer->getModel()->getPosition());
	localPlayer->getModel()->getHitBox()->setRotationAroundCenter(-cam->getYaw() + cam->getDefaultYaw());
	checkPlayerFloorColision(localPlayer, timeStamp);

	//std::cout << localPlayer->getPosition().x << " " << localPlayer->getPosition().y << " " << localPlayer->getPosition().z << std::endl;


	shader.setVec3("viewPos", localPlayer->getCamera()->getPosition());
	shader.setMatrix("view", localPlayer->getCamera()->getViewMatrix());

	manageLights(shader);
	checkCollisions();

	manageEntity(playerEntity, timeStamp, shader);
	localPlayer->draw(shader);

	for (int i = 0; i < playerEntity->bullets.size(); i++)
	{
		BasicBullet* b = playerEntity->bullets[i];
		if (b->state == Entity::ENTITY_STATE::DEAD)
		{
			playerEntity->removeBullet(i);
			deleteEntity(b);
		}
		else {
			manageEntity(b, timeStamp, shader);
			checkLifeTime(b);
		}
	}
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
	for (Light* l : map->getLights())
	{
		l->draw(EZNgine::localPlayer->getCamera()->getViewMatrix(), projection);
	}
};

void GameManager::loopInstancied(Shader& shader, double timeStamp)
{

};

void GameManager::onInitialized(EZNgine* engine)
{
	world = engine->dynamicsWorld;
	map = new LevelDungeon(engine->shader);
	playerEntity = new PlayerEntity(engine->localPlayer);
	playerEntity->model->setPosition(map->playerSpawnPoint);

	triggerTest = new Trigger([&](void* data)
		{
			std::cout << "Trigger collided with localplayer" << std::endl;
			playerEntity->health = 0.0f;
		}, { 20, 5, 5 });


	triggerTest->onInit(world);
	for (int i = 0; i < 1; i++)
	{
		enemys.emplace_back(new Enemy(glm::vec3(-132.686, 24.2506, 133.566)));
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

		if (entityA) entityA->onCollide(entityB);
		else if (entityB) entityB->onCollide(entityA);
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

void GameManager::checkPlayerFloorColision(LocalPlayer* lp, double timeStamp)
{
	const btVector3 playerPosition = { lp->getPosition().x, lp->getPosition().y, lp->getPosition().z };
	const btVector3 playerPosDownVect = { lp->getPosition().x, lp->getPosition().y - 5.0f, lp->getPosition().z };

	if (!raycastWorld(playerPosition, playerPosDownVect)) //rien en dessous des pieds
	{
		lp->canJump = false;
		btVector3 gravity = btVector3(0, GRAVITY_Y, 0);
		for (auto* rigidBody : lp->getModel()->getRigidBodys()) {
			if (rigidBody->getGravity().getY() != GRAVITY_Y) {
				rigidBody->setGravity(gravity);
			}
		}
	}
	else
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

void GameManager::manageLights(Shader& shader)
{
	if (mouseEnabled)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else
	{
		firstMouse = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (!mouseEnabled) imGuiLights(shader, map->getLights(), EZNgine::localPlayer);
}

bool GameManager::raycastWorld(btVector3 Start, btVector3 End) {

	btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
	RayCallback.m_collisionFilterMask = btBroadphaseProxy::AllFilter;
	world->rayTest(Start, End, RayCallback);
	return RayCallback.hasHit();
}

void GameManager::imGuiLights(Shader& shader, std::vector<Light*>& lights, LocalPlayer* localPlayer)
{
	char* items[50];
	for (int i = 0; i < lights.size(); i++)
	{
		items[i] = new char[255];
		itoa(i, items[i], 10);
	}

	float pos[3] = {
		lights.at(comboCurrentItem)->getPosition().x,
		lights.at(comboCurrentItem)->getPosition().y,
		lights.at(comboCurrentItem)->getPosition().z
	};
	float ambiant[3] = {
		lights.at(comboCurrentItem)->getAmbiant().x,
		lights.at(comboCurrentItem)->getAmbiant().y,
		lights.at(comboCurrentItem)->getAmbiant().z
	};
	float diffuse[3] = {
		lights.at(comboCurrentItem)->getDiffuse().x,
		lights.at(comboCurrentItem)->getDiffuse().y,
		lights.at(comboCurrentItem)->getDiffuse().z
	};
	float spec[3] = {
		lights.at(comboCurrentItem)->getSpecular().x,
		lights.at(comboCurrentItem)->getSpecular().y,
		lights.at(comboCurrentItem)->getSpecular().z
	};
	float down[3] = {
		lights.at(comboCurrentItem)->getDirection().x,
		lights.at(comboCurrentItem)->getDirection().y,
		lights.at(comboCurrentItem)->getDirection().z
	};
	float linear = lights.at(comboCurrentItem)->getLinear();
	float quadra = lights.at(comboCurrentItem)->getQuadratic();
	bool isDir = lights.at(comboCurrentItem)->getIsDirectional();
	float cutoff = lights.at(comboCurrentItem)->getCutOff();

	ImGui::Begin("Lights");
	if (ImGui::Combo("Light index", &comboCurrentItem, items, lights.size(), 5))
	{
		for (int n = 0; n < lights.size(); n++)
		{
			const bool is_selected = (comboCurrentItem == n);
			if (ImGui::Selectable(items[n], is_selected))
				comboCurrentItem = n;

			if (is_selected) {
				pos[0] = lights.at(comboCurrentItem)->getPosition().x;
				pos[1] = lights.at(comboCurrentItem)->getPosition().y;
				pos[2] = lights.at(comboCurrentItem)->getPosition().z;

				ambiant[0] = lights.at(comboCurrentItem)->getAmbiant().x;
				ambiant[1] = lights.at(comboCurrentItem)->getAmbiant().y;
				ambiant[2] = lights.at(comboCurrentItem)->getAmbiant().z;

				diffuse[0] = lights.at(comboCurrentItem)->getDiffuse().x;
				diffuse[1] = lights.at(comboCurrentItem)->getDiffuse().y;
				diffuse[2] = lights.at(comboCurrentItem)->getDiffuse().z;

				spec[0] = lights.at(comboCurrentItem)->getSpecular().x;
				spec[1] = lights.at(comboCurrentItem)->getSpecular().y;
				spec[2] = lights.at(comboCurrentItem)->getSpecular().z;

				down[0] = lights.at(comboCurrentItem)->getDirection().x;
				down[1] = lights.at(comboCurrentItem)->getDirection().y;
				down[2] = lights.at(comboCurrentItem)->getDirection().z;

				linear = lights.at(comboCurrentItem)->getLinear();
				quadra = lights.at(comboCurrentItem)->getQuadratic();
				isDir = lights.at(comboCurrentItem)->getIsDirectional();
				cutoff = lights.at(comboCurrentItem)->getCutOff();
				ImGui::SetItemDefaultFocus();
			}
		}
	}
	ImGui::DragFloat3("Position", pos, 0.05f, -1000.0f, 1000.0f);
	ImGui::ColorEdit3("Ambiant", ambiant);

	if (lights.at(comboCurrentItem)->getIsDirectional()) {
		ImGui::DragFloat("cutoff", &cutoff, 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat3("Direction", down, 0.1f, -10.0f, 10.0f);
	}
	else
	{
		ImGui::ColorEdit3("Diffuse", diffuse);
		ImGui::ColorEdit3("Specular", spec);
		ImGui::SliderFloat("linear", &linear, -0.5f, 5.0f);
		ImGui::SliderFloat("quadra", &quadra, 0.0f, 10.0f);
	}

	ImGui::Checkbox("Directional", &isDir);
	if (lights.at(comboCurrentItem)->getIsDirectional() != isDir) {
		lights.at(comboCurrentItem)->setIsDirectional(isDir);
	}

	if (ImGui::Button("Set current position"))
	{
		lights.at(comboCurrentItem)->setPosition(localPlayer->getPosition());
	}
	else
	{
		lights.at(comboCurrentItem)->setPosition({ pos[0], pos[1], pos[2] });
	}
	if (ImGui::Button("Add light"))
	{
		const glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
		auto* light = new Light(&shader, localPlayer->getPosition(), white);
		lights.emplace_back(light);
	}

	lights.at(comboCurrentItem)->setAmbiant({ ambiant[0], ambiant[1], ambiant[2] });
	lights.at(comboCurrentItem)->setDiffuse({ diffuse[0], diffuse[1], diffuse[2] });
	lights.at(comboCurrentItem)->setSpecular({ spec[0], spec[1], spec[2] });
	lights.at(comboCurrentItem)->setDirection({ down[0], down[1], down[2] });
	lights.at(comboCurrentItem)->setLinear(linear);
	lights.at(comboCurrentItem)->setQuadratic(quadra);
	lights.at(comboCurrentItem)->setCutOff(cutoff);

	if (ImGui::Button("Save"))
	{
		nlohmann::json exportJson;
		for (int i = 0; i < lights.size(); i++) exportJson[i] = lights.at(i)->toJson();
		std::ofstream outStream("../../maps/lights.json");
		outStream << std::setw(4) << exportJson << std::endl;
	}
	ImGui::End();
}

void GameManager::setWindow(GLFWwindow* window)
{
	this->window = window;
}
