#include "IHitBox.h"

IHitBox::IHitBox()
{
}

IHitBox::~IHitBox()
{
}

glm::vec3 IHitBox::getCenter() const
{
	return center;
}
