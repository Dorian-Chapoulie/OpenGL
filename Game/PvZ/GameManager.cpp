#include "GameManager.h"

#include <thread>

#include "LocalPlayer.h"
#include <imgui/imgui.h>

#include "Animator.h"

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

	//std::cout << playerEntity->getLife() << std::endl;
	//std::cout << localPlayer->getPosition().x << " " << localPlayer->getPosition().y << " " << localPlayer->getPosition().z << std::endl;

	shader.setVec3("viewPos", localPlayer->getCamera()->getPosition());
	shader.setMatrix("view", localPlayer->getCamera()->getViewMatrix());

	manageLights(shader);
	checkCollisions();

	manageEntity(playerEntity, timeStamp, shader, true);
	localPlayer->draw(shader);

	if (door != nullptr) door->draw(shader);

	if (boss != nullptr)
	{
		if (boss->getLife() <= 0) boss->state = Entity::ENTITY_STATE::DEAD;

		if (boss->state == Entity::ENTITY_STATE::DEAD)
		{
			for (int i = 0; i < boss->bullets.size(); i++) {
				BasicBullet* b = boss->bullets[i];
				boss->removeBullet(i);
				deleteEntity(b);
			}
			deleteEntity(boss);
		}
		else {
			manageEntity(boss, timeStamp, shader, true);
			for (int i = 0; i < boss->bullets.size(); i++) {
				BasicBullet* b = boss->bullets[i];
				if (b->state == Entity::ENTITY_STATE::DEAD)
				{
					boss->removeBullet(i);
					deleteEntity(b);
				}
				else {
					manageEntity(b, timeStamp, shader, true);
					checkLifeTime(b);
				}
			}
		}
	}

	for (int i = 0; i < playerEntity->bullets.size(); i++)
	{
		PlayerBullet* b = playerEntity->bullets[i];
		if (b->state == Entity::ENTITY_STATE::DEAD)
		{
			playerEntity->removeBullet(i);
			deleteEntity(b);
		}
		else {
			manageEntity(b, timeStamp, shader, true);
			checkLifeTime(b);
		}
	}
	for (Enemy* e : enemys) {
		if (e->getLife() <= 0) e->state = Entity::ENTITY_STATE::DEAD;

		if (e->state == Entity::ENTITY_STATE::DEAD)
		{
			for (int i = 0; i < e->bullets.size(); i++) {
				BasicBullet* b = e->bullets[i];
				e->removeBullet(i);
				deleteEntity(b);
			}
			deleteEntity(e);
			continue;
		}

		manageEntity(e, timeStamp, shader, true);
		for (int i = 0; i < e->bullets.size(); i++) {
			BasicBullet* b = e->bullets[i];
			if (b->state == Entity::ENTITY_STATE::DEAD)
			{
				e->removeBullet(i);
				deleteEntity(b);
			}
			else {
				manageEntity(b, timeStamp, shader, true);
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

	if (playerEntity->getLife() <= 0)
	{
		onLocalPlayerDie();
	}
};

void GameManager::loopInstancied(Shader& shader, double timeStamp)
{

}

void GameManager::loopAnimated(Shader& s, double timeStamp)
{
	for (Actor* a : actors)
	{
		a->model->draw(s, *a->animator, EZNgine::localPlayer->getCamera()->getViewMatrix());
	}
};

void GameManager::onInitialized(EZNgine* engine)
{
	world = engine->dynamicsWorld;
	map = new LevelDungeon(engine->shader);
	playerEntity = new PlayerEntity(engine->localPlayer);
	playerEntity->model->setPosition(map->playerSpawnPoint);

	initTrigers();


	std::array<glm::vec3, 7> enemysSpawnPoints = {
		glm::vec3(-295.075f, 35, 169.402f),
		glm::vec3(-299.401, 35, 227.285),
		glm::vec3(-138.936, 35, 282.963),
		glm::vec3(-303.846, 35, 429.078),
		glm::vec3(-214.091, 35, 373.078),
		glm::vec3(-161.147, 35, 531.068),
		glm::vec3(-258.391, 35, 521.575)
	};
	for (const auto& vec : enemysSpawnPoints) {
		enemys.emplace_back(new Enemy(vec));
		enemys.back()->onInit(world);
	}

	for (auto* rigidBody : EZNgine::localPlayer->getModel()->getRigidBodys()) {
		world->addRigidBody(rigidBody);
	}
	for (auto* rigidBody : map->getModel()->getRigidBodys()) {
		world->addRigidBody(rigidBody);
	}

	actors.emplace_back(new Actor(new Model("../../models/erika/erika.dae", { -70, 16.5f, 25 }, glm::vec3(0.2f), true)));
	actors.emplace_back(new Actor(new Model("../../models/gilbert/gilbert.dae", { -121, 16, -120 }, glm::vec3(0.2f), true)));
	actors.emplace_back(new Actor(new Model("../../models/karen/karen.dae", { -350, 16, -70 }, glm::vec3(0.2f), true)));
	actors.emplace_back(new Actor(new Model("../../models/orc/orc.dae", { -225, 16.5f, -77 }, glm::vec3(0.2f), true)));

	actors[0]->model->setRotation({ 0, 1, 0 }, glm::radians(-90.0f));
	actors[2]->model->setRotation({ 0, 1, 0 }, glm::radians(90.0f));

	irrklang::vec3df position(actors[0]->model->getPosition().x, actors[0]->model->getPosition().y, actors[0]->model->getPosition().z);
	auto s = EZNgine::soundEngine->play3D(std::string("../../audio/ambiance/0.wav").c_str(), position, true, false, true);


	std::thread t([&]()
		{
			int i = 0;
			while (isPlaying)
			{
				i += 10;
				for (Actor* a : actors)
				{
					a->animator->UpdateAnimation(0.05f);
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
				if (i == 100)
				{
					irrklang::vec3df position(actors[0]->model->getPosition().x, actors[0]->model->getPosition().y, actors[0]->model->getPosition().z);
					EZNgine::soundEngine->play3D(std::string("../../audio/actors/erika.wav").c_str(), position);
				}
				else if (i == 200)
				{
					irrklang::vec3df position(actors[2]->model->getPosition().x, actors[2]->model->getPosition().y, actors[2]->model->getPosition().z);
					EZNgine::soundEngine->play3D(std::string("../../audio/actors/karen.wav").c_str(), position);
				}
				else if (i == 300)
				{
					irrklang::vec3df position(actors[3]->model->getPosition().x, actors[3]->model->getPosition().y, actors[3]->model->getPosition().z);
					EZNgine::soundEngine->play3D(std::string("../../audio/actors/orc.wav").c_str(), position);
				}
				else if (i == 400)
				{
					irrklang::vec3df position(actors[1]->model->getPosition().x, actors[1]->model->getPosition().y, actors[1]->model->getPosition().z);
					EZNgine::soundEngine->play3D(std::string("../../audio/actors/zombie.wav").c_str(), position);
				}
			}
		});
	t.detach();
}

void GameManager::manageEntity(Entity* e, double timeStamp, Shader& shader, bool shouldDraw)
{
	e->onUpdate(timeStamp, world);
	if (shouldDraw) e->model->draw(shader);
}

void GameManager::checkCollisions()
{
	const int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++) {
		const btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		if (!obA || !obB)
		{
			std::cout << "avoid crash" << std::endl;
			return;
		}

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

void GameManager::onLocalPlayerDie()
{
	playerEntity->setLife(100.0f);
	playerEntity->model->setPosition(map->playerSpawnPoint);
	initTrigers();
}

void GameManager::initTrigers()
{
	if (door != nullptr)
	{
		door->setPosition({ 0, -100, 0 });
	}
	if (boss != nullptr)
	{
		boss->canShoot = false;
		boss->model->setPosition({ 0, -200, 0 });
		for (BasicBullet* b : boss->bullets)
		{
			deleteEntity(b);
		}
		boss->bullets.clear();
	}

	triggerTest = new Trigger([&](void* data)
		{
			irrklang::vec3df position(playerEntity->model->getPosition().x, playerEntity->model->getPosition().y, playerEntity->model->getPosition().z);
			auto s = EZNgine::soundEngine->play3D(std::string("../../audio/trigger/2.mp3").c_str(), position, true, false, true);


			if (boss == nullptr) {
				boss = new Enemy({ -214.901, 135.939, 747.199 });
				boss->setLife(500.0f);
				for (auto* rigidBody : boss->model->getRigidBodys()) {
					world->addRigidBody(rigidBody);
				}
			}
			else
			{
				boss->canShoot = true;
				boss->setLife(500.0f);
				boss->model->setPosition({ -214.901, 135.939, 747.199 });
			}
			if (door == nullptr) {
				door = new StaticModel(doorPath, { -258.084, 120.004, 640.802 }, HitBoxFactory::AABB, glm::vec3(20.0f));
				for (auto* rigidBody : door->getRigidBodys()) {
					world->addRigidBody(rigidBody);
				}
			}
			else
			{
				door->setPosition({ -258.084, 120.004, 640.802 });
			}
			deleteEntity(triggerTest);
			delete triggerTest;
		}, { -270.915, 104.992, 645.611 }, 10.0f);
	triggerTest->onInit(world);
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
