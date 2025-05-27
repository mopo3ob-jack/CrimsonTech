#ifndef CT_FACE_HPP
#define CT_FACE_HPP

#include <mstd/geometry>

namespace ct {

class Face {
public:
	mstd::Vector3f normal() const {
		return mstd::normalize(mstd::cross(vertices[0] - vertices[2], vertices[0] - vertices[1]));
	}

	mstd::Vector3f* vertices;
	mstd::Size count;
};

}

#endif
