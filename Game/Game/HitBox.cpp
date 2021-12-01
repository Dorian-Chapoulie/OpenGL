#include "HitBox.h"
#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>
#include <Bullet3/btBulletDynamicsCommon.h>
#include "MyMotionState.h"


HitBox::HitBox(Model* model) : model(model)
{
	setup_hitbox();
}

HitBox::HitBox(Model* model, std::vector<Vertex>& vertices) : model(model)
{
	setup_hitbox(vertices);
}


HitBox::~HitBox()
{

}

glm::vec3 HitBox::get_center() const
{
	return center_;
}

btRigidBody* HitBox::get_rigid_body()
{
	return rigid_body_;
}

Model* HitBox::get_model()
{
	return model;
}

void HitBox::set_rotation_around_center(const float angle)
{
	this->center_rotation = glm::radians(angle);
	btQuaternion quat;
	quat.setEuler(center_rotation, 0, 0);
	quat.setRotation(btVector3(0, 1, 0), center_rotation);
	rigid_body_->getWorldTransform().setRotation(quat);
}

void HitBox::set_world_transform(const glm::vec3& position, const glm::quat& rot)
{
	//glm::quat r = glm::quat(glm::vec3(0, centerRotation, 0));
	model->set_position(position);
	glm::mat4* modelMatrix = this->model->get_model_matrix();
	*modelMatrix = glm::translate(glm::mat4(1.0f), position);
	*modelMatrix *= glm::rotate(glm::mat4(1.0f), center_rotation, glm::vec3(0.f, 1.f, 0.f));
}

//TODO: set linear velocity for some
void HitBox::setup_hitbox()
{
	const std::array<glm::vec3, 2> dataSize = findBiggestMesh();
	size_ = dataSize[0] * glm::vec3(0.5f);
	center_ = dataSize[1];

	box_collision_shape_ = new btBoxShape(btVector3(size_.x, size_.y, size_.z));
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(center_.x, center_.y, center_.z));

	rigid_body_ = new btRigidBody(
		btRigidBody::btRigidBodyConstructionInfo(
			weight_,
			nullptr,
			box_collision_shape_
		)
	);

	rigid_body_->setMotionState(new MyMotionState(this, transform));
	rigid_body_->setCenterOfMassTransform(transform);
	//TODO: find best way
	rigid_body_->setActivationState(DISABLE_DEACTIVATION);
}

void HitBox::setup_hitbox(std::vector<Vertex>& vertices)
{
	const std::array<glm::vec3, 2> dataSize = get_mesh_center_and_size(vertices);
	size_ = dataSize[0] * glm::vec3(0.5f);
	center_ = dataSize[1];

	box_collision_shape_ = new btBoxShape(btVector3(size_.x, size_.y, size_.z));
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(center_.x, center_.y, center_.z));

	rigid_body_ = new btRigidBody(
		btRigidBody::btRigidBodyConstructionInfo(
			weight_,
			nullptr,
			box_collision_shape_
		)
	);

	rigid_body_->setMotionState(new MyMotionState(this, transform));
	rigid_body_->setCenterOfMassTransform(transform);
	//TODO: find best way
	rigid_body_->setActivationState(DISABLE_DEACTIVATION);
}

std::array<glm::vec3, 2> HitBox::findBiggestMesh() const
{
	auto sizeCenter = get_mesh_center_and_size(model->get_data().meshes[0]->getVertices());
	auto maxSize = glm::vec3(sizeCenter[0]);
	//TODO: refactor with std
	for (Mesh* m : model->get_data().meshes)
	{
		const auto temp = get_mesh_center_and_size(m->getVertices());
		const glm::vec3 size = temp[0];
		if (size.x > maxSize.x && size.y > maxSize.y && size.z > maxSize.z)
		{
			maxSize = size;
			sizeCenter = temp;
		}
		m->cleanUp();
	}
	return sizeCenter;
}

std::array<glm::vec3, 2> HitBox::get_mesh_center_and_size(const std::vector<Vertex>& vertices_) const
{
	//TODO: with std max_element
	GLfloat max_x, max_y, max_z;
	GLfloat min_x = max_x = vertices_.at(0).Position.x;
	GLfloat min_y = max_y = vertices_.at(0).Position.y;
	GLfloat min_z = max_z = vertices_.at(0).Position.z;
	for (int i = 1; i < vertices_.size(); i++) {
		if (vertices_.at(i).Position.x < min_x) min_x = vertices_.at(i).Position.x;
		if (vertices_.at(i).Position.x > max_x) max_x = vertices_.at(i).Position.x;
		if (vertices_.at(i).Position.y < min_y) min_y = vertices_.at(i).Position.y;
		if (vertices_.at(i).Position.y > max_y) max_y = vertices_.at(i).Position.y;
		if (vertices_.at(i).Position.z < min_z) min_z = vertices_.at(i).Position.z;
		if (vertices_.at(i).Position.z > max_z) max_z = vertices_.at(i).Position.z;
	}
	const glm::vec3 scale = this->model->get_scale();
	const glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z) * scale;
	const glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2) * scale + model->get_position();

	return std::array{ size, center };
}
