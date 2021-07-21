#include "MyMotionState.h"

MyMotionState::MyMotionState(btRigidBody* body, glm::vec3 center, glm::vec3 scale, Model* model) : m_body(body), m_model(model) {
    // Note the negative sign here: we're computing the offset required to go from mesh-frame to body-frame
    btVector3 mesh_center = btVector3(center.x, center.y, center.z);

    btVector3 mesh_scale = btVector3(scale.x, scale.y, scale.z);

    btTransform identity;
    btVector3 aabb_min, aabb_max;
    m_body->getCollisionShape()->getAabb(identity, aabb_min, aabb_max);
    btVector3 body_scale = aabb_max - aabb_min;

    // Note: btVector3 divided by btVector3 is a component-wise inversion
    // ideally you would guard against division by zero here
    // but that is left as an exercise for the reader
    m_scaleMeshToBody = body_scale / mesh_scale;

    // Note the negative sign here: it is the offset that must be applied to the mesh
    // to put it where the body is, post scale;
    m_scaledMeshOffsetToBody = -mesh_center * m_scaleMeshToBody;
}

void MyMotionState::getWorldTransform(btTransform& worldTrans) const {
    // Note: worldTrans is passed by reference
    // it is the duty of MyMotionState to update worldTrans with the correct value the body should have
    // and the Bullet API will apply the transform it gets from this method to the body

    // compute worldTrans here, as per scripted motion or some other authority
    //worldTrans = ...
    btTransform tr;
    tr.setIdentity();
    tr.setOrigin(btVector3(m_model->position.x, m_model->position.y, m_model->position.z));
    worldTrans = tr * btTransform::getIdentity().inverse();
}


void MyMotionState::setWorldTransform(const btTransform& worldTrans) {
    // converting btQuaternion to glm::quat is left as an exercise for the reader
    //glm::quat rotation = ...
    btTransform tr = worldTrans * btTransform::getIdentity().inverse();
    glm::vec3 position(tr.getOrigin().getX(), tr.getOrigin().getY(), tr.getOrigin().getZ());
    m_model->setPosition(position);
    printf("%f %f %f\n", position.x, position.y, position.z);
}