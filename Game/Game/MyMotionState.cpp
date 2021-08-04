#include "MyMotionState.h"

MyMotionState::MyMotionState(
    Model* model,
    const btTransform& startTrans,
    const btTransform& centerOfMassOffset
)
    : model(model),
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

    const glm::vec3 center = model->getCenter();
    const glm::vec3 basePosition = model->getBasePosition();

    const glm::vec3 tmp = glm::vec3(
        position.x - center.x + basePosition.x,
        position.y - center.y + basePosition.y,
        position.z - center.z + basePosition.z
    );

    model->setWorldTransform(tmp);
    m_graphicsWorldTrans = worldTrans * m_centerOfMassOffset;
}