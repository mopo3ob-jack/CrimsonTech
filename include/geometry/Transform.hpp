#ifndef HDB_CT_TRANSFORM_HPP
#define HDB_CT_TRANSFORM_HPP

#include "../tensor/tensor.hpp"

namespace hdb {

namespace ct {

class Transform {
	Transform getGlobalTransform() const {
		if (parent) {
			Transform start = parent->getGlobalTransform();
			Transform ret;
		} else {
			return *this;
		}
	}

	Transform transform(Transform transform) const {
		
	}

	Transform* parent = nullptr;

	Vector3f position;
	Vector3f scale;
	Vector4f rotation; //Stored as a rotor
};

}

}

#endif