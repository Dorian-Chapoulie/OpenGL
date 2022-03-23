#include "EZNgine.h"
#include "GameManager.h"
#define DRAW_DISTANCE 5000.0f
#define FOV 50.0f

int main()
{
	GameManager g;
	glm::mat4 projection = glm::perspective(glm::radians(FOV), 800.0f / 600.0f, 0.1f, DRAW_DISTANCE);
	EZNgine engine(projection, &g);

	return 0;
}
