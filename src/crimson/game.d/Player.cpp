#include <crimson/game.d/Player.hpp>

namespace ct {

void Player::update(BSP& bsp) {
	using namespace mstd;
	using namespace ct;

	constexpr F32 gravity = -9.81f;
	constexpr F32 frictionCoefficient = 1.7f;
	constexpr F32 dragCoefficient = 0.425f;
	constexpr F32 groundAcceleration = 40.0f;
	constexpr F32 airAcceleration = 10.0f;
	constexpr F32 maxVelocity = 6.0f;

	Vector3f acceleration = Vector3f(0.0f);
	if (keyMask & FORWARD) {
		acceleration += Vector3f(std::sin(angle.y), 0.0f, std::cos(angle.y));
	}

	if (keyMask & BACKWARD) {
		acceleration -= Vector3f(std::sin(angle.y), 0.0f, std::cos(angle.y));
	}

	if (keyMask & RIGHT) {
		acceleration += Vector3f(std::cos(angle.y), 0.0f, -std::sin(angle.y));
	}

	if (keyMask & LEFT) {
		acceleration -= Vector3f(std::cos(angle.y), 0.0f, -std::sin(angle.y));
	}

	if (magnitude(acceleration) > 1.0f) {
		acceleration = normalize(acceleration);
	}

	if (airFrames) {
		acceleration *= airAcceleration;
	} else {
		if (keyMask & JUMP) {
			velocity.y = 5.0f;
		}
		acceleration *= groundAcceleration;
	}

	++airFrames;
	velocity += acceleration * Time::deltaTime;
	velocity.y += gravity * Time::deltaTime;
	Vector3f targetPosition = position + velocity * Time::deltaTime;
	BSP::Trace result;
	while (bsp.clip(position, targetPosition, result)) {
		targetPosition = position + velocity * Time::deltaTime;
		velocity -= result.plane.normal * dot(result.plane.normal, velocity);

		if (result.plane.normal.y > 0.1f) {
			airFrames = 0;
		}
	}
	position = result.point;

	Vector3f hVelocity = velocity;
	hVelocity.y = 0.0f;
	if (magnitude(hVelocity) > maxVelocity && airFrames == 0) {
		hVelocity = normalize(hVelocity) * maxVelocity;
	}

	Vector3f friction;
	if (airFrames) {
		friction = normalize(hVelocity) * gravity * dragCoefficient * Time::deltaTime;
	} else {
		friction = normalize(hVelocity) * gravity * frictionCoefficient * Time::deltaTime;
	}
	if (magnitude(friction) < magnitude(hVelocity)) {
		hVelocity += friction;
	} else {
		hVelocity = Vector3f(0.0f);
	}

	velocity.x = hVelocity.x;
	velocity.z = hVelocity.z;
}

}
