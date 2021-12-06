#pragma once
#include <Bullet3/btBulletCollisionCommon.h>
#include <Bullet3/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include "Model.h"

class MyMotionState : public btMotionState {
private:
	Model* model = nullptr;
	btTransform m_graphicsWorldTrans;
	btTransform m_centerOfMassOffset;
	btTransform m_startWorldTrans;

public:
	MyMotionState(
		Model* model,
		const btTransform& startTrans = btTransform::getIdentity(),
		const btTransform& centerOfMassOffset = btTransform::getIdentity()
	);

	void getWorldTransform(btTransform& worldTrans) const override;
	void setWorldTransform(const btTransform& worldTrans) override;
};
