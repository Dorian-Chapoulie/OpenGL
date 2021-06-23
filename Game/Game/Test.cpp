#include "Test.h"

#include <glm/fwd.hpp>

Test::Test()
{
}

void Test::onContact(const CallbackData& callbackData)
{
	// For each contact pair
	for (glm::uint p = 0; p < callbackData.getNbContactPairs(); p++) {
		// Get the contact pair
		CollisionCallback::ContactPair contactPair = callbackData.getContactPair(p);

		std::cout << contactPair.getBody1()->getUserData() << " with: " << *reinterpret_cast<std::string*>(contactPair.getBody2()->getUserData()) << std::endl;
		
		// For each contact point of the contact pair
		/*for (glm::uint c = 0; c < contactPair.getNbContactPoints(); c++) {
			// Get the contact point
			CollisionCallback::ContactPoint contactPoint = contactPair.getContactPoint(c);

			reactphysics3d::Vector3 worldPoint = contactPair.getCollider1() ->getLocalToWorldTransform() * contactPoint.getLocalPointOnCollider1();
	
		}*/
	}
}
