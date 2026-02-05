#ifndef CT_MATERIALLOADER_HPP
#define CT_MATERIALLOADER_HPP

#include <crimson/renderer.d/StorageBuffer.hpp>

namespace ct {

class MaterialLoader {
public:
	MaterialLoader() {}

private:
	StorageBuffer materialTable;
};

}

#endif
