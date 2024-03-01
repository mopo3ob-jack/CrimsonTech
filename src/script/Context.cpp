#include "../../include/script/Context.hpp"

namespace hdb {

namespace ct {
	
namespace Context {

GLFWwindow* window;
GLFWmonitor* monitor;
const GLFWvidmode* mode;
			
double Time::time;
float Time::deltaTime;
float Time::fps;

Vector2f Input::mousePosition;
Vector2f Input::mouseDelta;
double Input::mouseX;
double Input::mouseY;

}

}

}