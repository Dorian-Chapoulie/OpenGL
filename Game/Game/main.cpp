#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <irrKlang/irrKlang.h>
#include "Light.h"
#include "Model.h"
#include <thread>
#include <reactphysics3d/reactphysics3d.h>

#include "LocalPlayer.h"
#include "Test.h"

#define WIDTH 800
#define HEIGHT 600
#define FULLSCREEN false

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;
float mouseX, mouseY;

LocalPlayer* localPlayer = nullptr;
irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();

/*
 * model[0][0] => WIDTH
 * model[1][1] => HEIGHT
 * model[3][0] => x
 * model[3][1] => y
 * model[3][2] => z
 * [3][3] => max
 *
 * TODO: DESTRUCTEUR glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteProgram(shaderProgram);
 */

class Line {
	int shaderProgram;
	unsigned int VBO, VAO;
	std::vector<float> vertices;
	glm::vec3 startPoint;
	glm::vec3 endPoint;
	glm::mat4 MVP;
	glm::vec3 lineColor;
public:
	Line(glm::vec3 start, glm::vec3 end) {

		startPoint = start;
		endPoint = end;
		lineColor = glm::vec3(1, 1, 1);

		const char* vertexShaderSource = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"uniform mat4 MVP;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			"}\0";
		const char* fragmentShaderSource = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"uniform vec3 color;\n"
			"void main()\n"
			"{\n"
			"   FragColor = vec4(color, 1.0f);\n"
			"}\n\0";

		// vertex shader
		int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);
		// check for shader compile errors

		// fragment shader
		int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);
		// check for shader compile errors

		// link shaders
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		// check for linking errors

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		vertices = {
			 start.x, start.y, start.z,
			 end.x, end.y, end.z,
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	void setMVP(glm::mat4 mvp) {
		MVP = mvp;
	}

	void setColor(glm::vec3 color) {
		lineColor = color;
	}

	int draw() {
		glUseProgram(shaderProgram);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
		glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);

		glBindVertexArray(VAO);
		//glLineWidth(10.0f);
		glDrawArrays(GL_LINES, 0, 2);
		return 0;
	}

	~Line() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteProgram(shaderProgram);
	}
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	const static std::unique_ptr<Camera>& cam = localPlayer->getCamera();
	const float cameraSpeed = 10.0f * deltaTime;
	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam->increasePosition(cameraSpeed * cam->getFrontVector());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam->increasePosition(-cameraSpeed * cam->getFrontVector());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam->increasePosition(-glm::normalize(glm::cross(cam->getFrontVector(), cam->getUpVector())) * cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam->increasePosition(glm::normalize(glm::cross(cam->getFrontVector(), cam->getUpVector())) * cameraSpeed);

	const float step = 0.001f;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		
	}
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		
	}
	else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		
	}
	else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
	{
		
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
	cam->setDirection(direction);
}

void setupShader(Shader& shader)
{
	const glm::mat4 model = glm::mat4(1.0f);
	const glm::mat4 view = localPlayer->getCamera()->getViewMatrix();
	const glm::mat4 projection = glm::perspective(glm::radians(70.0f), static_cast<float>(WIDTH / HEIGHT), 0.1f, 30.0f);

	shader.use();
	shader.setMatrix("model", model);
	shader.setMatrix("view", view);
	shader.setVec3("viewPos", localPlayer->getCamera()->getPosition());
	shader.setMatrix("projection", projection);

	shader.setVec3("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("material.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setValue<float>("material.shininess", 50.0f);
}

void createLights(Shader& shader)
{
	const glm::vec3 downVector = glm::vec3(0.0f, -1.0f, 0.0f);
	const glm::vec3 white = glm::vec3(1.0f);
	const glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 blue = glm::vec3(0.0f, 1.0f, 1.0f);
	const glm::vec3 green = glm::vec3(0.0f, 1.0f, 0.0f);

	auto* light = new Light(&shader, glm::vec3(5.75878, 1.32539, -5.09563), white);

	auto* light2 = new Light(&shader, glm::vec3(-3, 3, 0), downVector, blue, 100.0f);
	auto* light4 = new Light(&shader, glm::vec3(0.5, 3, 0), downVector, red, 100.0f);
	auto* light5 = new Light(&shader, glm::vec3(4, 3, 0), downVector, green, 100.0f);
}

void setupSound()
{
	//SoundEngine->play2D("../../audio/quake/standard/prepare.mp3", false);
	//irrklang::vec3df position(0.0f, 0.8f, -8.0f);
	//SoundEngine->play3D("../../audio/quake/standard/monsterkill.mp3", position);
}

void ray()
{
	/*const glm::mat4 projection = glm::perspective(glm::radians(70.0f), static_cast<float>(WIDTH / HEIGHT), 0.1f, 30.0f);
	
	float x = (2.0f * mouseX) / WIDTH - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / HEIGHT;
	float z = 1.0f;
	glm::vec3 ray_nds = glm::vec3(x, y, z);
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	glm::vec3 ray_wor = glm::vec3(0.0f);
	ray_wor.x = (glm::inverse(localPlayer->getCamera()->getViewMatrix()) * ray_eye).x;
	ray_wor.y = (glm::inverse(localPlayer->getCamera()->getViewMatrix()) * ray_eye).y;
	ray_wor.z = (glm::inverse(localPlayer->getCamera()->getViewMatrix()) * ray_eye).z;
	
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);*/
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
#pragma endregion endsetup

	localPlayer = new LocalPlayer("../../models/crate/Wooden Crate.obj", "../../models/M4a1/M4a1.obj", glm::vec3(0.0f, 0.8f, -8.0f));
	//Model model("../../models/aim_deagle7k/map.obj");
	//Model model("../../models/crate/Wooden Crate.obj");
	Model model("../../models/M4a1/M4a1.obj");
	//Model model("../../models/floor/CobbleStones2.obj", glm::vec3(0.0f, -2.0f, 0.0f));
	//Model model("../../models/crate/Wooden Crate.obj", glm::vec3(0.0f, 0.0f, 0.0f));
	//Model model2("../../models/crate/Wooden Crate.obj", glm::vec3(4.0f, 0.0f, 0.0f));
	//Model model("../../models/sphere/sphere.obj", glm::vec3(4.0f, 0.0f, 0.0f));
	
	Shader shader("./vertex.vert", "./fragment.frag");

	setupShader(shader);
	setupSound();
	createLights(shader);

	reactphysics3d::PhysicsCommon physicsCommon;
	reactphysics3d::PhysicsWorld* world = physicsCommon.createPhysicsWorld();
	
	world->setIsDebugRenderingEnabled(true);
	reactphysics3d::DebugRenderer& debugRenderer = world->getDebugRenderer();
	debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_POINT, true);
	debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::CONTACT_NORMAL, true);
	debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
	debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, true);
	debugRenderer.setIsDebugItemDisplayed(reactphysics3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
	
	reactphysics3d::Vector3 position(0.0, 0.0, 0.0);
	reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
	glm::vec3 size = model.meshes[0].getBoundingBow()[0];
	glm::vec3 center = model.meshes[0].getBoundingBow()[1];
	reactphysics3d::Transform transform(reactphysics3d::Vector3(center.x, center.y, center.z), orientation);
	reactphysics3d::CollisionBody* body = world->createCollisionBody(transform);


	reactphysics3d::Transform transform2(position, orientation);
	reactphysics3d::CollisionBody* body2 = world->createCollisionBody(transform2);

	float radius = 30.0f;
	//reactphysics3d::SphereShape * sphereShape = physicsCommon.createSphereShape(radius);
	reactphysics3d::Vector3 vec = reactphysics3d::Vector3(size.x / 2, size.y / 2, size.z / 2);
	reactphysics3d::BoxShape* boxShape = physicsCommon.createBoxShape(vec);
	//reactphysics3d::Collider* collider = body->addCollider(boxShape, transform);
	reactphysics3d::Collider* collider2 = body2->addCollider(boxShape, transform);

	std::string s = "test";
	Test test;
	reactphysics3d::CollisionCallback& cb = test;
	body2->setUserData(&s);
	const static std::unique_ptr<Camera>& cam = localPlayer->getCamera();

	const glm::mat4 projection = glm::perspective(glm::radians(70.0f), static_cast<float>(WIDTH / HEIGHT), 0.1f, 30.0f);
	//Line l(glm::vec3(center.x - size.x / 2, center.y, center.z), glm::vec3(0, -5, 0));
	std::vector<Line*> lines;
	
	float timeStep = 1.0 / 60.0f;
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //POUR AVOIR LES TRAIT DES VERTICES
	while (!glfwWindowShouldClose(window))
	{
		world->update(timeStep);
		world->testCollision(body2, body2, cb);
		
		reactphysics3d::Vector3 vec(cam->getPosition().x, cam->getPosition().y, cam->getPosition().z);
		reactphysics3d::Transform tmp(vec,orientation);
		body->setTransform(tmp);

		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		int debugLines = debugRenderer.getNbLines();
		for (int i = 0; i < debugLines; i++)
		{
			reactphysics3d::DebugRenderer::DebugLine l = debugRenderer.getLinesArray()[i];
			lines.emplace_back(new Line(glm::vec3(l.point1.x, l.point1.y, l.point1.z), glm::vec3(l.point2.x, l.point2.y, l.point2.z)));
			//lines.emplace_back(new Line(glm::vec3(0, 0, 0), glm::vec3(5, 0, 0)));
			
			/*std::cout << l.point1.x << " " << l.point1.y << " " << l.point1.y << std::endl;
			std::cout << l.point2.x << " " << l.point2.y << " " << l.point2.y << std::endl;
			std::cout << std::endl;*/
		}
		for (Line* l : lines)
		{
			l->setMVP(projection * localPlayer->getCamera()->getViewMatrix());
			l->draw();
			delete l;
		}
		lines.clear();

		//l.setMVP(projection * localPlayer->getCamera()->getViewMatrix());
		//l.draw();

		shader.use();

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		shader.setVec3("viewPos", localPlayer->getCamera()->getPosition());
		localPlayer->draw(shader);
		shader.setMatrix("view", localPlayer->getCamera()->getViewMatrix());
		model.draw(shader);
		//model2.draw(shader);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}