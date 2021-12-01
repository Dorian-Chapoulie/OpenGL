#pragma once
#include <array>
#include <vector>
#include <Bullet3/btBulletCollisionCommon.h>

#include "Model.h"
#include "Vertex.h"

class HitBox
{
public:

	HitBox(Model* model);
	HitBox(Model* model, std::vector<Vertex>& vertices);
	~HitBox();

	[[nodiscard]] glm::vec3 get_center() const;
	[[nodiscard]] btRigidBody* get_rigid_body();
	[[nodiscard]] Model* get_model();

	void set_rotation_around_center(const float angle);
	void set_world_transform(const glm::vec3& position, const glm::quat& rot);

private:
	Model* model;

	void setup_hitbox();
	void setup_hitbox(std::vector<Vertex>& vertices);

	btVector3 m_scaledMeshOffsetToBody;
	//TODO: set a factory for collision shapes
	btCollisionShape* box_collision_shape_ = nullptr;
	btRigidBody* rigid_body_ = nullptr;

	float weight_ = 0.0f;
	float center_rotation = 0.0f;
	glm::vec3 center_;
	glm::vec3 size_;

	[[nodiscard]] std::array<glm::vec3, 2> findBiggestMesh() const;
	[[nodiscard]] std::array<glm::vec3, 2> get_mesh_center_and_size(const std::vector<Vertex>& vertices_) const;
};

