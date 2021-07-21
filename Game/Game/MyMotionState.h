#pragma once
#include <Bullet3/btBulletCollisionCommon.h>
#include <Bullet3/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include "Model.h"

class MyMotionState : public btMotionState {
    // Note: we assume no rotational transform between the mesh and the body
    // and only consider translation and scale.
    btVector3 m_scaledMeshOffsetToBody;
    btVector3 m_scaleMeshToBody;
    btRigidBody* m_body;
    Model* m_model;

public:
    MyMotionState(btRigidBody* body, glm::vec3 center, glm::vec3 size, Model* model);

    // getWorldTransform() relays incoming position/rotation to the RigidBody
    // and is called on active Kinematic objects at each substep and also          
    // once when the btRigidBody is initialized with a btRigidBodyConstructionInfo.
    // If you don't want the body's transform to be set by the MotionState at                  
    // construction time then give it a null MotionState and then subsequently                                                     
    // explicitly call body->setMotionState(motionState)
    //
    void getWorldTransform(btTransform& worldTrans) const;

    // setWorldTransform() relays outgoing position/rotation to the Renderable
    // and is called on active Dynamic objects at each substep
    //
    void setWorldTransform(const btTransform& worldTrans);
};
