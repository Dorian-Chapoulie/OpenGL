#include "EZNgine.h"
#include "BaseApplication.h"
#include "LevelKinoDerToten.h"
#define DRAW_DISTANCE 5000.0f
#define FOV 50.0f


class Game : public BaseApplication
{
private:
	bool forward = false, backward = false, left = false, right = false, jump = false;
	LevelKinoDerToten* map;
	StaticModel* enemy = nullptr;
public:

	void processInput(void* w) override
	{
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

	void loop(Shader& shader, double timeStamp) override
	{
		LocalPlayer* localPlayer = EZNgine::localPlayer;
		std::unique_ptr<Camera>& cam = localPlayer->getCamera();
		localPlayer->move(forward, backward, left, right, jump, timeStamp);
		localPlayer->setCameraPosition(localPlayer->getModel()->getPosition());
		localPlayer->getModel()->getHitBox()->setRotationAroundCenter(-cam->getYaw() + cam->getDefaultYaw());



		shader.setVec3("viewPos", localPlayer->getCamera()->getPosition());
		shader.setMatrix("view", localPlayer->getCamera()->getViewMatrix());

		localPlayer->draw(shader);
		enemy->draw(shader);
		map->draw();
	};

	void loopInstancied(Shader& shader, double timeStamp) override
	{

	};

	void onInitialized(EZNgine* engine) override
	{
		map = new LevelKinoDerToten(engine->shader);
		enemy = new StaticModel("../../models/shooter/shooter.fbx", glm::vec3(0.0f, 6.0f, 0.0f), HitBoxFactory::AABB, glm::vec3(0.01f));

		for (auto* rigidBody : EZNgine::localPlayer->getModel()->getRigidBodys()) {
			engine->dynamicsWorld->addRigidBody(rigidBody);
		}
		for (auto* rigidBody : map->getModel()->getRigidBodys()) {
			engine->dynamicsWorld->addRigidBody(rigidBody);
		}
		for (auto* rigidBody : enemy->getRigidBodys()) {
			engine->dynamicsWorld->addRigidBody(rigidBody);
		}
	}
};

int main()
{
	Game g;
	glm::mat4 projection = glm::perspective(glm::radians(FOV), 800.0f / 600.0f, 0.1f, DRAW_DISTANCE);
	EZNgine engine(projection, &g);



	return 0;
}
