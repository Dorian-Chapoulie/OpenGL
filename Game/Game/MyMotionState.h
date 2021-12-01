#pragma once
#include <Bullet3/btBulletCollisionCommon.h>
#include "HitBox.h"

class MyMotionState : public btMotionState {
public:
	MyMotionState(
		HitBox* body,
		const btTransform& startTrans = btTransform::getIdentity(),
		const btTransform& centerOfMassOffset = btTransform::getIdentity()
	);

	void getWorldTransform(btTransform& worldTrans) const override;
	void setWorldTransform(const btTransform& worldTrans) override;

private:
	HitBox* body = nullptr;
	btTransform m_graphicsWorldTrans;
	btTransform m_centerOfMassOffset;
	btTransform m_startWorldTrans;
};
