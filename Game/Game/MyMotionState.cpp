#include "MyMotionState.h"
#include <glm/detail/type_quat.hpp>

MyMotionState::MyMotionState(
	HitBox* body,
	const btTransform& startTrans,
	const btTransform& centerOfMassOffset
)
	: body(body),
	m_graphicsWorldTrans(startTrans),
	m_centerOfMassOffset(centerOfMassOffset),
	m_startWorldTrans(startTrans) {

}

void MyMotionState::getWorldTransform(btTransform& worldTrans) const {
	worldTrans = m_graphicsWorldTrans * m_centerOfMassOffset.inverse();
}


void MyMotionState::setWorldTransform(const btTransform& worldTrans) {
	glm::vec3 position(
		worldTrans.getOrigin().getX(),
		worldTrans.getOrigin().getY(),
		worldTrans.getOrigin().getZ()
	);

	const glm::vec3 center = body->get_center();
	const glm::vec3 basePosition = body->get_model()->get_base_position();

	const glm::vec3 tmp = glm::vec3(
		position.x - center.x + basePosition.x,
		position.y - center.y + basePosition.y,
		position.z - center.z + basePosition.z
	);

	const btQuaternion worldTransformQuaternion = worldTrans.getRotation();
	const glm::quat rotation = glm::quat(glm::vec3(0, abs(worldTransformQuaternion.getY()), 0));

	body->set_world_transform(tmp, rotation);
	m_graphicsWorldTrans = worldTrans * m_centerOfMassOffset;
}