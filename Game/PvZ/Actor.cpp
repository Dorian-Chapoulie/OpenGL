#include "Actor.h"

Actor::Actor(Model* m) : model(m)
{
	animator = new Animator(m->getAnimation(), m);
}
