#include "EZNgine.h"
#include "GameManager.h"
#define DRAW_DISTANCE 500.0f
#define FOV 50.0f

int main()
{
	constexpr float WIDTH = 1920.0f;
	constexpr float HEIGHT = 1080.0f;
	constexpr bool isFullScreen = true;

	EZNgine::WIDTH = WIDTH;
	EZNgine::HEIGHT = HEIGHT;
	EZNgine::FULLSCREEN = isFullScreen;

	glm::mat4 projection = glm::perspective(glm::radians(FOV), WIDTH / HEIGHT, 0.1f, DRAW_DISTANCE);
	GameManager g(projection);
	EZNgine engine(projection, &g);
	g.setWindow(engine.window);
	engine.loop();

	return 0;
}
