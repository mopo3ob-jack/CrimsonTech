#include <crimson/game.d/FreeCamera.hpp>
#include <crimson/game.d/Time.hpp>

namespace ct {

void FreeCamera::update() {
	using namespace mstd;
	using namespace ct;

	constexpr F32 maxVelocity = 3.0f;

	Vector3f velocity(0.0f);
	if (keyMask & FORWARD) {
		velocity += Vector3f(viewMatrix[0].y, viewMatrix[1].y, viewMatrix[2].y);
	}

	if (keyMask & BACKWARD) {
		velocity -= Vector3f(viewMatrix[0].y, viewMatrix[1].y, viewMatrix[2].y);
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
		velocity.z += maxVelocity;
	}

	if (keyMask & DOWN) {
		velocity.z -= maxVelocity;
	}

	position += velocity * Time::deltaTime;

	Camera::update();
}

}