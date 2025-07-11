#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <mstd/geometry>
#include <crimson/file>

class Player {
public:
	void update(mstd::F32 deltaTime, ct::BSP& bsp) {
		using namespace mstd;
		using namespace ct;

		constexpr F32 gravity = -9.81f;
		constexpr F32 frictionCoefficient = 1.7f;
		constexpr F32 maxVelocity = 6.0f;

		if (colliding) {
			//velocity -= result.plane.normal * dot(result.plane.normal, velocity);
			//acceleration -= result.plane.normal * dot(result.plane.normal, acceleration);
		}

		result.velocity = velocity;
		Vector3f targetPosition = position + velocity * deltaTime;
		colliding = bsp.intersect(position, targetPosition, result);
		position = result.point;
		velocity = result.velocity;

		if (colliding) {
			//acceleration -= result.plane.normal * dot(result.plane.normal, acceleration);
		}
		velocity += acceleration * deltaTime;

		Vector3f hVelocity = velocity;
		hVelocity.y = 0.0f;
		if (magnitude(hVelocity) > maxVelocity) {
			hVelocity = normalize(hVelocity) * maxVelocity;
			velocity.x = hVelocity.x;
			velocity.z = hVelocity.z;
		}

		Vector3f friction = normalize(velocity) * gravity * frictionCoefficient * deltaTime;
		if (magnitude(friction) < magnitude(velocity)) {
			velocity += friction;
		} else {
			velocity = Vector3f(0.0f);
		}
	}

	mstd::Vector3f position, velocity, acceleration;
	ct::BSP::Trace result;
	mstd::Bool colliding;
};

#endif
