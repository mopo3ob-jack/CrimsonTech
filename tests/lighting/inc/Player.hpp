#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <mstd/geometry>
#include <crimson/file>
#include <imgui/imgui.h>

class Player {
public:
	void update(mstd::F32 deltaTime, ct::BSP& bsp) {
		using namespace mstd;
		using namespace ct;

		if (!ImGui::Begin("Player")) {
			ImGui::End();
			return;
		}

		constexpr F32 gravity = -9.81f;
		constexpr F32 frictionCoefficient = 1.7f;
		constexpr F32 maxVelocity = 6.0f;

		velocity += acceleration * deltaTime;
		velocity.y += gravity * deltaTime;
		Vector3f targetPosition = position + velocity * deltaTime;
		while (bsp.clip(position, targetPosition, result)) {
			targetPosition = position + velocity * deltaTime;
			velocity -= result.plane.normal * dot(result.plane.normal, velocity);
		}
		position = result.point;

		Vector3f hVelocity = velocity;
		hVelocity.y = 0.0f;
		if (magnitude(hVelocity) > maxVelocity) {
			hVelocity = normalize(hVelocity) * maxVelocity;
			velocity.x = hVelocity.x;
			velocity.z = hVelocity.z;
		}

		Vector3f friction = normalize(hVelocity) * gravity * frictionCoefficient * deltaTime;
		if (magnitude(friction) < magnitude(hVelocity)) {
			hVelocity += friction;
		} else {
			hVelocity = Vector3f(0.0f);
		}

		velocity.x = hVelocity.x;
		velocity.z = hVelocity.z;

		ImGui::Text("position = %s", std::string(position).c_str());
		ImGui::Text("velocity = %s | %f", std::string(velocity).c_str(), magnitude(velocity));
		ImGui::Text("acceleration = %s | %f", std::string(acceleration).c_str(), magnitude(acceleration));
		
		ImGui::End();
	}

	mstd::Vector3f position, velocity, acceleration;
	ct::BSP::Trace result;
};

#endif
