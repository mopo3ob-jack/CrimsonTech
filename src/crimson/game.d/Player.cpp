#include <crimson/game.d/Player.hpp>

namespace ct {

void Player::update(BSP& bsp) {
	using namespace mstd;
	using namespace ct;

	constexpr F32 gravity = -9.81f;
	constexpr F32 frictionCoefficient = 2.0f;
	constexpr F32 drag = -10.0f;
	constexpr F32 groundAcceleration = 60.0f;
	constexpr F32 airAcceleration = 10.0f;
	constexpr F32 maxVelocity = 5.0f;
	constexpr F32 EPSILON = 1.0f / 4096.0f;

	Vector3f acceleration = Vector3f(0.0f);
	if (keyMask & FORWARD) {
		acceleration += Vector3f(viewMatrix[0].y, viewMatrix[1].y, 0.0f);
	}

	if (keyMask & BACKWARD) {
		acceleration -= Vector3f(viewMatrix[0].y, viewMatrix[1].y, 0.0f);
	}

	if (keyMask & RIGHT) {
		acceleration += Vector3f(viewMatrix[0].x, viewMatrix[1].x, 0.0f);
	}

	if (keyMask & LEFT) {
		acceleration -= Vector3f(viewMatrix[0].x, viewMatrix[1].x, 0.0f);
	}

	if (magnitude(acceleration) > 1.0f) {
		acceleration = normalize(acceleration);
	}

	if (airFrames) {
		acceleration *= airAcceleration;
	} else {
		if (keyMask & JUMP) {
			velocity.z = 5.0f;
		}
		acceleration *= groundAcceleration;
	}

	acceleration.z = gravity;

	++airFrames;
	velocity += acceleration * Time::deltaTime;

	if (magnitude(velocity) == 0.0f) {
		Camera::update();
		return;
	}

	Vector3f delta = velocity * Time::deltaTime;
	Vector3f targetPosition = position + delta;
	Vector3f friction = Vector3f(0.0f);
	BSP::Trace result;
	for (U32 i = 0; i < 6 && bsp.clip(position, targetPosition, result); ++i) {
		delta -= result.plane.normal * (result.plane.distance(targetPosition) - EPSILON);

		for (Size i = 0; i < 3; ++i) {
			if (std::abs(delta[i]) < EPSILON) {
				delta[i] = 0.0f;
			}
		}

		velocity = delta / Time::deltaTime;

		position = result.point;

		if (result.plane.normal.z > result.plane.normal.x && result.plane.normal.z > result.plane.normal.y) {
			airFrames = 0;

			if (magnitude(delta) == 0.0f) {
				friction = -delta;
				break;
			}

			friction = normalize(delta) * (gravity * frictionCoefficient * Time::deltaTime * Time::deltaTime);
			if (magnitude(friction) > magnitude(delta)) {
				friction = -delta;
				break;
			}
		}

		targetPosition = position + delta + friction;
	}
	velocity += friction / Time::deltaTime;
	position = result.point;

	Vector3f hVelocity = velocity;
	hVelocity.z = 0.0f;
	if (magnitude(hVelocity) > maxVelocity) {
		hVelocity = normalize(hVelocity) * maxVelocity;
	}
	velocity.x = hVelocity.x;
	velocity.y = hVelocity.y;

	Camera::update();
}

}
