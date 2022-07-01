#include "EZNgine.h"
#include "GameManager.h"
//TODO: change this value 
#define DRAW_DISTANCE 500.0f
#define FOV 70.0f

int main()
{
	int WIDTH = 800;
	int HEIGHT = 600;

	glm::mat4 projection = glm::perspective(glm::radians(FOV), float(WIDTH / HEIGHT), 0.1f, DRAW_DISTANCE);
	GameManager g(projection);
	EZNgine engine(projection, &g);
	//g.setWindow(engine.window);
	engine.loop();

	return 0;
}
