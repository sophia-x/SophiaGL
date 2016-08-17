#include <tools/callback_helper>

#include <unordered_map>
using namespace std;

namespace CallBackHelper {

double scroll_x, scroll_y;
GLFWwindow* scroll_window;
unordered_map<unsigned long, bool> scrolled_map;

void scrollCallBack(GLFWwindow *window, double x, double y) {
	scroll_x = x;
	scroll_y = y;
	scroll_window = window;
	for (auto it = scrolled_map.begin(); it != scrolled_map.end(); ++it) {
		it->second = true;
	}
}

void regist(unsigned long key) {
	scrolled_map[key] = false;
}

double getScrollX() {
	return scroll_x;
}

double getScrollY() {
	return scroll_y;
}

const GLFWwindow* getWindow() {
	return scroll_window;
}

bool scrolled(unsigned long key) {
	return scrolled_map.at(key);
}

void debugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void* userParam) {
	fprintf(stderr, "OpenGL Debug Output message:\n");

	if (source == GL_DEBUG_SOURCE_API_ARB)
		fprintf(stderr, "Source : API; ");
	else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)
		fprintf(stderr, "Source : WINDOW_SYSTEM; ");
	else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)
		fprintf(stderr, "Source : SHADER_COMPILER; ");
	else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)
		fprintf(stderr, "Source : THIRD_PARTY; ");
	else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)
		fprintf(stderr, "Source : APPLICATION; ");
	else if (source == GL_DEBUG_SOURCE_OTHER_ARB)
		fprintf(stderr, "Source : OTHER; ");

	fprintf(stderr, "\n");

	if (type == GL_DEBUG_TYPE_ERROR_ARB)
		fprintf(stderr, "Type : ERROR; ");
	else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)
		fprintf(stderr, "Type : DEPRECATED_BEHAVIOR; ");
	else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)
		fprintf(stderr, "Type : UNDEFINED_BEHAVIOR; ");
	else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)
		fprintf(stderr, "Type : PORTABILITY; ");
	else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)
		fprintf(stderr, "Type : PERFORMANCE; ");
	else if (type == GL_DEBUG_TYPE_OTHER_ARB)
		fprintf(stderr, "Type : OTHER; ");

	fprintf(stderr, "\n");

	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)
		fprintf(stderr, "Severity : HIGH; ");
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)
		fprintf(stderr, "Severity : MEDIUM; ");
	else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)
		fprintf(stderr, "Severity : LOW; ");

	fprintf(stderr, "\n");

	fprintf(stderr, "Message: %s\n", message);
}

}