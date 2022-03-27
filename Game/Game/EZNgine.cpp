#include "EZNgine.h"

#include <irrKlang/irrKlang.h>

#include "GLDebugDrawer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

float lastX = 0.0f, lastY = 0.0f;
float mouseX = 0.0f, mouseY = 0.0f;

LocalPlayer* EZNgine::localPlayer = nullptr;
BaseApplication* EZNgine::base_application = nullptr;
float EZNgine::WIDTH = 800.0f;
float EZNgine::HEIGHT = 600.0f;
bool EZNgine::FULLSCREEN = false;

EZNgine::EZNgine(const glm::mat4 projection, BaseApplication* base_application)
	: projection(projection)
{
	EZNgine::base_application = base_application;
	this->setupOpenGl();
	this->setupBulletPhysics();
	this->init();
	base_application->onInitialized(this);
}

void EZNgine::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void EZNgine::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (!base_application->mouseEnabled) return;

	if (base_application->firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		base_application->firstMouse = false;
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

void EZNgine::scroll_callback(GLFWwindow* window, double xpos, double ypos)
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

void EZNgine::loop()
{
	float timeStep = 1 / 10.0f;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		dynamicsWorld->stepSimulation(timeStep, 10);
		dynamicsWorld->debugDrawWorld();

		const float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		base_application->processInput(window);

		shader.use();
		base_application->loop(shader, deltaTime);

		instancedShader.use();
		base_application->loopInstancied(instancedShader, deltaTime);

		animationShader.use();
		base_application->loopAnimated(animationShader, deltaTime);


#pragma region SKYBOX
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		glm::mat4 view = glm::mat4(glm::mat3(localPlayer->getCamera()->getViewMatrix()));
		skyboxShader.setMatrix("view", view);
		skyboxShader.setMatrix("projection", projection);
		skybox->draw();
#pragma endregion
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();
}

void EZNgine::setupShader(Shader& s)
{
	const glm::mat4 model = glm::mat4(1.0f);
	const glm::mat4 view = localPlayer->getCamera()->getViewMatrix();

	s.use();
	s.setMatrix("model", model);
	s.setMatrix("view", view);
	s.setVec3("viewPos", localPlayer->getCamera()->getPosition());
	s.setMatrix("projection", projection);

	s.setVec3("material.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
	s.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	s.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	s.setValue<float>("material.shininess", 100.0f);
}

void EZNgine::setupSkyBoxShader()
{
	const glm::mat4 view = localPlayer->getCamera()->getViewMatrix();
	skyboxShader.setValue<int>("skybox", 0);
	skyboxShader.setMatrix("view", view);
	skyboxShader.setMatrix("projection", projection);
}

void EZNgine::setupSound()
{
	soundEngine = irrklang::createIrrKlangDevice();
	//SoundEngine->play2D("../../audio/quake/standard/prepare.mp3", false);
	//irrklang::vec3df position(0.0f, 0.8f, -8.0f);
	//SoundEngine->play3D("../../audio/quake/standard/monsterkill.mp3", position);
}

void EZNgine::setupOpenGl()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_REFRESH_RATE, 144);

	window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", FULLSCREEN ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}


	glfwSetFramebufferSizeCallback(window, &EZNgine::framebuffer_size_callback);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, &EZNgine::mouse_callback);
	glfwSetScrollCallback(window, &EZNgine::scroll_callback);
	glfwSetCursorPos(window, WIDTH / 2.0f, HEIGHT / 2.0f);
}

void EZNgine::setupBulletPhysics()
{
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The actual physics solver
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	// The world.
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0.0f, -9.0f, 0.0f));
	GLDebugDrawer* debugDraw = new GLDebugDrawer();
	debugDraw->DBG_DrawWireframe;
	debugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	//dynamicsWorld->setDebugDrawer(debugDraw);
}

void EZNgine::setupImGui()
{
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
}

void EZNgine::promptFps(int& nbFrames, double& lastTime)
{
	const double currentTime = glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 1.0) {
		std::cout << nbFrames << " FPS\n";
		nbFrames = 0;
		lastTime += 1.0;
	}
}

void EZNgine::init()
{
	EZNgine::localPlayer = new LocalPlayer("../../models/cube/cube.obj", glm::vec3(0.0f, 6.0f, 0.0f));

	shader = Shader("vertex.vert", "fragment.frag");
	instancedShader = Shader("instancedVert.vert", "instancedFrag.frag");
	skyboxShader = Shader("skybox.vert", "skybox.frag");
	skybox = new SkyBox("../../textures/skybox");
	animationShader = Shader("./animation.vert", "./animation.frag");

	this->setupShader(animationShader);
	this->setupShader(shader);
	this->setupImGui();
	this->setupSkyBoxShader();
	this->setupSound();
}

