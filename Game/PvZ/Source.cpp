#include "EZNgine.h"
#include "GameManager.h"
#define DRAW_DISTANCE 500.0f
#define FOV 50.0f

int main()
{
	constexpr float WIDTH = 800.0f;
	constexpr float HEIGHT = 600.0f;
	constexpr bool isFullScreen = false;

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
