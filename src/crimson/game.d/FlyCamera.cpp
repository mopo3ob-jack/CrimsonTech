#include <crimson/game.d/FlyCamera.hpp>
#include <crimson/game.d/Time.hpp>

namespace ct {

void FlyCamera::update(BSP& bsp) {
	using namespace mstd;
	using namespace ct;

	constexpr F32 maxVelocity = 3.0f;

	Vector3f velocity(0.0f);
	if (keyMask & FORWARD) {
		velocity += Vector3f(viewMatrix[0].z, viewMatrix[1].z, viewMatrix[2].z);
	}

	if (keyMask & BACKWARD) {
		velocity -= Vector3f(viewMatrix[0].z, viewMatrix[1].z, viewMatrix[2].z);
	}

	if (keyMask & RIGHT) {
		velocity += Vector3f(viewMatrix[0].x, viewMatrix[1].x, viewMatrix[2].x);
	}

	if (keyMask & LEFT) {
		velocity -= Vector3f(viewMatrix[0].x, viewMatrix[1].x, viewMatrix[2].x);
	}

	if (magnitude(velocity) > 1.0f) {
		velocity = normalize(velocity);
	}

	velocity *= maxVelocity;

	if (keyMask & UP) {
		velocity.y += maxVelocity;
	}

	if (keyMask & DOWN) {
		velocity.y -= maxVelocity;
	}

	Vector3f targetPosition = position + velocity * Time::deltaTime;
	BSP::Trace result;
	for (U32 i = 0; i < 16 && bsp.clip(position, targetPosition, result); ++i) {
		targetPosition = position + velocity * Time::deltaTime;
		velocity -= result.plane.normal * dot(result.plane.normal, velocity);
	}
	position = result.point;

	Camera::update();
}

}
