#ifndef HDB_CT_PHYSICSCONTROLLERNOROT_HPP
#define HDB_CT_PHYSICSCONTROLLERNOROT_HPP

#include "../tensor/tensor.hpp"

namespace hdb {

namespace ct {

class PhysicsControllerNoRot {
public:
	PhysicsControllerNoRot(float mass = 1.0f, float friction = 0.5f, float elasticity = 0.5f)
		: mass(mass), invMass(1.0f / mass), friction(friction), elasticity(elasticity) {
		velocity = Vector3f(0.0f);
	}

	constexpr void applyForce(Vector3f force) {
		velocity += force * invMass;
	}

	const float mass = 1.0f;
	const float invMass = 1.0f;
	const float friction = 0.5f;
	const float elasticity = 0.5f;

	Vector3f position, velocity;
};

}

}

#endif