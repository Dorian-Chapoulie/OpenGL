#pragma once
#include <reactphysics3d/reactphysics3d.h>
class Test :
    public reactphysics3d::CollisionCallback
{
	
public:
	Test();
	void onContact(const CallbackData& callbackData) override;
};

