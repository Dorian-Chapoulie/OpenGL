#include <excpt.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nlohmann/json.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
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
#include "InstancedModel.h"
#include "LevelKinoDerToten.h"
#include "SkeletalLoader.h"
#include "StaticModel.h"

#define WIDTH 800
#define HEIGHT 600
#define FULLSCREEN false
//TODO: change this value 
#define DRAW_DISTANCE 5000.0f
#define FOV 50.0f

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
bool mouseEnabled = true;
bool canUpdateMouse = true;

LocalPlayer* localPlayer = nullptr;
irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();

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
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && canUpdateMouse)
	{
		canUpdateMouse = false;
		std::thread([]()
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(700));
				canUpdateMouse = true;
			}).detach();

			mouseEnabled = !mouseEnabled;
			if (mouseEnabled)
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else
			{
				firstMouse = true;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
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
	if (mouseEnabled) cam->setDirection(direction);
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

	auto* light = new Light(&shader, glm::vec3(0.f, 50.0f, 0.f), white);
	lights.emplace_back(light);

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

void drawInstancing(Shader& shader, Model* rock, int amount, glm::mat4 projection)
{
	// draw meteorites
	shader.use();
	shader.setMatrix("projection", projection);
	shader.setMatrix("view", localPlayer->getCamera()->getViewMatrix());
	for (unsigned int i = 0; i < rock->getModelData()->meshes.size(); i++)
	{
		glBindVertexArray(rock->getModelData()->meshes[i]->VAO);
		glDrawElementsInstanced(
			GL_TRIANGLES,
			rock->getModelData()->meshes[i]->indices.size(),
			GL_UNSIGNED_INT,
			0,
			amount
		);
	}
}

bool RaycastWorld(btDiscreteDynamicsWorld* dynamicsWorld, btVector3 Start, btVector3 End, btVector3& Normal) {


	btCollisionWorld::ClosestRayResultCallback RayCallback(Start, End);
	RayCallback.m_collisionFilterMask = btBroadphaseProxy::AllFilter;

	//std::cout << Start.getX() << " " << Start.getY() << " " << Start.getZ() << std::endl;
	//std::cout << End.getX() << " " << End.getY() << " " << End.getZ() << std::endl << std::endl;
	dynamicsWorld->rayTest(Start, End, RayCallback);
	if (RayCallback.hasHit()) {

		End = RayCallback.m_hitPointWorld;
		Normal = RayCallback.m_hitNormalWorld;
		return true;
	}


	return false;
}

int comboCurrentItem = 0;
void imGuiLights(Shader& shader, std::vector<Light*>& lights)
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
		std::ofstream outStream("lights.json");
		outStream << std::setw(4) << exportJson << std::endl;
	}
	ImGui::End();
}

float get3DDistance(btVector3 a, btVector3 b)
{
	return abs(sqrt(
		pow(abs(a.getX() - b.getX()), 2) +
		pow(abs(a.getY() - b.getY()), 2) +
		pow(abs(a.getZ() - b.getZ()), 2)
	));
}

void performAstar(std::vector<btRigidBody*>& path, btVector3 start, btVector3 end, std::vector<btRigidBody*> beacons, btDiscreteDynamicsWorld* dynamicsWorld)
{
	constexpr int NODES_AROUND = 10;
	//Récupérer node de fin
	btRigidBody* endNode = *std::min_element(beacons.begin(), beacons.end(), [&](btRigidBody* a, btRigidBody* b) {
		return get3DDistance(a->getWorldTransform().getOrigin(), end) < get3DDistance(b->getWorldTransform().getOrigin(), end);
	});

	//Récupérer node de début
	btRigidBody* startNode = *std::min_element(beacons.begin(), beacons.end(), [&](btRigidBody* a, btRigidBody* b) {
		return get3DDistance(a->getWorldTransform().getOrigin(), start) < get3DDistance(b->getWorldTransform().getOrigin(), start);
	});

	//Pour tout les nodes autour u noeud de debut
	std::sort(beacons.begin(), beacons.end(), [&](btRigidBody* a, btRigidBody* b) {
		const btVector3 startNodeVector = startNode->getWorldTransform().getOrigin();
		return get3DDistance(a->getWorldTransform().getOrigin(), startNodeVector) < get3DDistance(b->getWorldTransform().getOrigin(), startNodeVector);
	});

	std::vector<btRigidBody*> beaconsCopy = beacons;
	std::vector<std::vector<btRigidBody*>> results;
	for (int i = 0; i < NODES_AROUND; i++) {
		results.emplace_back(std::vector<btRigidBody*>());

		//Récupérer node de début
		btRigidBody* startNodeTmp = beaconsCopy[i];
		results[i].emplace_back(startNodeTmp);
		beaconsCopy.erase(std::remove_if(beaconsCopy.begin(),
			beaconsCopy.end(),
			[&](btRigidBody* a) {return (int)beaconsCopy[i]->getUserPointer() == (int)a->getUserPointer(); }),
			beaconsCopy.end()
		);

		while ((int)results[i].back()->getUserPointer() != (int)endNode->getUserPointer()) {

			std::vector<btRigidBody*> tmp;
			for (int j = 0; j < beaconsCopy.size(); j++) {
				if (j >= NODES_AROUND) break;
				tmp.emplace_back(beaconsCopy[j]);
			}

			//Trie les noeuds proche de moi en fonction de leurs distance eu noeud de fin
			std::sort(tmp.begin(), tmp.end(), [&](btRigidBody* a, btRigidBody* b) {
				const btVector3 endNodeVector = endNode->getWorldTransform().getOrigin();
				return get3DDistance(a->getWorldTransform().getOrigin(), endNodeVector) < get3DDistance(b->getWorldTransform().getOrigin(), endNodeVector);
			});
			if (tmp.size() <= 0) break;

			//Ajouter le noeud le plus proche de end parmis mes voisins
			bool shouldBreak = true;
			for (int j = 0; j < tmp.size(); j++) {
				if ((int)tmp[j]->getUserPointer() != (int)startNodeTmp->getUserPointer() && abs(endNode->getWorldTransform().getOrigin().getY() - tmp[j]->getWorldTransform().getOrigin().getY()) <= 5.0) {
					results[i].emplace_back(tmp[j]);

					beaconsCopy.erase(std::remove_if(beaconsCopy.begin(),
						beaconsCopy.end(),
						[&](btRigidBody* a) {return  (int)tmp[j]->getUserPointer() == (int)a->getUserPointer(); }),
						beaconsCopy.end()
					);
					shouldBreak = false;
					break;
				}
			}
			if (shouldBreak) break;
		}
		beaconsCopy = beacons;
	}

	std::vector<btRigidBody*> res = *std::min_element(results.begin(), results.end(), [](std::vector<btRigidBody*> a, std::vector<btRigidBody*> b) {
		return a.size() < b.size();
	});

	for (int i = 0; i < results.size(); i++) {
		//std::cout << i << ": " << results[i].size() << std::endl;
	}

	path.emplace_back(startNode);
	for (btRigidBody* r : res) path.emplace_back(r);
}

void moveEnemy(std::vector<btRigidBody*> p, btVector3 enemyPosbt, DynamicModel& enemy)
{
	if (p.size() <= 0) return;
	std::vector<btRigidBody*> path = { p.begin(), p.end() };

	btVector3 diff = { 0.0f, 0.0f, 0.0f };

	path.erase(std::remove_if(path.begin(),
		path.end(),
		[&](btRigidBody* a) { return get3DDistance(enemyPosbt, a->getWorldTransform().getOrigin()) <= 3; }),
		path.end());
	if (path.size() <= 0) return;

	diff = (path[0]->getWorldTransform().getOrigin() - enemyPosbt);
	diff *= 0.2f;


	for (auto* rigidBody : enemy.getRigidBodys()) {
		rigidBody->activate();
		rigidBody->setLinearVelocity(btVector3(
			diff.getX(),
			0,
			diff.getZ()
		));
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
	//dynamicsWorld->setDebugDrawer(debugDraw);
#pragma endregion physics

#pragma region imgui
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
#pragma endregion imgui

	//std::vector<Light*> lights;
	//../../models/floor_2/floor.obj
	//StaticModel model("../../models/css/css.dae", glm::vec3(0.0f, 0.0f, 0.0f), HitBoxFactory::AABB_MULTIPLE, glm::vec3(5.0f));
	//StaticModel model("../../models/kino/kino.obj", glm::vec3(0.0f, 0.0f, 0.0f), HitBoxFactory::TRIANGLE, glm::vec3(2.0f));
	//DynamicModel model3("../../models/crate/Wooden Crate.obj", glm::vec3(-4.0f, -5.0f, 40.4f), 1.0f, HitBoxFactory::AABB);
	//../../models/manequin/manequin_3.fbx
	InstancedModel modelInstanced("../../models/cube/cube.obj", 100);
	localPlayer = new LocalPlayer("../../models/cube/cube.obj", glm::vec3(-20.4472f, -5.9984f, 80.152f));

	DynamicModel enemy("../../models/cube/cube.obj", glm::vec3(-20.4472f, -5.9984f, 80.152f), 1.0f, HitBoxFactory::AABB);

	std::vector<Model*> dynamicModels;


	Shader shader("./vertex.vert", "./fragment.frag");
	Shader instancedShader("./instancedVert.vert", "./instancedFrag.frag");
	Shader skyboxShader("./skybox.vert", "./skybox.frag");
	Shader animationShader("./animation.vert", "./animation.frag");
	SkyBox skybox("../../textures/skybox");

	glm::mat4 projection = glm::perspective(glm::radians(FOV), 16.0f / 9.0f, 0.1f, DRAW_DISTANCE);

	setupShader(animationShader, projection);
	setupShader(shader, projection);
	setupSkyBoxShader(skyboxShader, projection);
	setupSound();
	//createLights(shader, lights);

	LevelKinoDerToten map(shader);


	void initParticles();

	btVector3 g = btVector3(0, -5, 0);
	for (auto* rigidBody : localPlayer->getModel()->getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
		rigidBody->setGravity(g);
	}
	for (auto* rigidBody : map.getModel()->getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
	}
	for (auto* rigidBody : enemy.getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
	}
	/*for (DynamicModel* m : dynamicModels)
	{
		for (auto* rigidBody : m->getRigidBodys()) {
			dynamicsWorld->addRigidBody(rigidBody);
		}
	}*/
	/*for (auto* rigidBody : model3.getRigidBodys()) {
		dynamicsWorld->addRigidBody(rigidBody);
	}*/

	//Animator animator(model3.getAnimation(), &model3);
	for (int i = 0; i < 100; i++)
	{
		glm::vec3 pos = { i, 0, 0 };
		dynamicModels.emplace_back(
			new Model("../../models/cube/cube.obj", pos)
		);
	}
	std::thread t([&]()
		{
			int dx = 1;
			float i = 0;
			std::vector<btRigidBody*> path;
			std::vector<btRigidBody*> beacons = map.getNavigationBodys();
			while (!jump) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}

			while (true) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				btVector3 playerPosition = { localPlayer->getPosition().x, localPlayer->getPosition().y, localPlayer->getPosition().z };
				btVector3 enemyPosition = enemy.getRigidBodys()[0]->getWorldTransform().getOrigin();
				performAstar(path, enemyPosition, playerPosition, beacons, dynamicsWorld);

				for (int i = 0; i < 100; i++)
				{
					glm::vec3 pos = { 0,-100,0 };
					dynamicModels[i]->setPosition(pos);
				}
				for (int i = 0; i < dynamicModels.size(); i++)
				{
					if (i >= path.size()) break;
					glm::vec3 pos = { path[i]->getWorldTransform().getOrigin().getX(),
					path[i]->getWorldTransform().getOrigin().getY(),
					path[i]->getWorldTransform().getOrigin().getZ() };
					dynamicModels[i]->setPosition(pos);
				}
				moveEnemy(path, enemyPosition, enemy);
				path.clear();

				//model3.setPosition(glm::vec3(0, 3, -5 + i));
				//model3.setRotation(glm::vec3(0, 1, 0), i += 0.1f);
				//animator.UpdateAnimation(deltaTime);
			}
		});

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
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//modelInstanced.draw(instancedShader, projection, localPlayer->getCamera()->getViewMatrix());
		const glm::vec3 pp = localPlayer->getPosition();

		btVector3 playerPos = btVector3(pp.x, pp.y, pp.z);
		btVector3 playerPos2 = btVector3(pp.x, pp.y - 2.0f, pp.z);
		btVector3 normal;

		/*if (!RaycastWorld(dynamicsWorld, playerPos, playerPos2, normal)) //rien en dessous des pieds
		{
			g = btVector3(0, -5, 0);
			for (auto* rigidBody : localPlayer->getModel()->getRigidBodys()) {
				if (rigidBody->getGravity().getY() != -5) {
					rigidBody->setGravity(g);
				}
			}
		}
		else
		{
			g = btVector3(0, 0, 0);
			for (auto* rigidBody : localPlayer->getModel()->getRigidBodys()) {
				if (rigidBody->getGravity().getY() != 0) {
					rigidBody->setGravity(g);
				}
			}
		}*/


		dynamicsWorld->stepSimulation(timeStep, 10);
		dynamicsWorld->debugDrawWorld();

		const float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//promptFps(nbFrames, lastTime);


		localPlayer->move(forward, backward, left, right, jump, deltaTime);
		localPlayer->setCameraPosition(localPlayer->getModel()->getPosition());
		localPlayer->getModel()->getHitBox()->setRotationAroundCenter(-cam->getYaw() + cam->getDefaultYaw());

		//model3.draw(animationShader, animator, localPlayer->getCamera()->getViewMatrix());

		for (Light* l : map.getLights())
		{
			l->draw(localPlayer->getCamera()->getViewMatrix(), projection);
		}

		shader.use();
		if (!mouseEnabled) imGuiLights(shader, map.getLights());

		shader.setVec3("viewPos", localPlayer->getCamera()->getPosition());
		shader.setMatrix("view", localPlayer->getCamera()->getViewMatrix());

		localPlayer->draw(shader);
		map.draw();
		enemy.draw(shader);
		//model3.draw(shader);

		for (Model* m : dynamicModels)
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
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();
	return 0;
}