#include <scene/WindowManager>

namespace gl
{

unique_ptr<WindowManager> WindowManager::manager_ptr;
once_flag WindowManager::created;

WindowManager& WindowManager::getWindowManager() {
	call_once(created,
	[] {
		manager_ptr.reset(new WindowManager);
	});
	return *manager_ptr.get();
}

WindowManager::WindowManager() {
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		throw "Window manager: can not init GLFW";
	}
}

void WindowManager::turnGlewOn() {
	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		throw "Window manager: can not init GLEW";
	}
}

void WindowManager::turnDebugOn(void (*p_debugOutputCallback)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*)) {
	if ( GLEW_ARB_debug_output ) {
		fprintf(stderr, "OpengL Debug turned on.\n");
		glDebugMessageCallbackARB(p_debugOutputCallback, NULL);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	} else {
		fprintf(stderr, "ARB_debug_output unavailable.\n");
	}
}

namespace Helper {

GLFWwindow* createWindow(unsigned int width, unsigned int height, const string &title) {
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

	// Open a window and create its OpenGL context
	GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		throw "Creatw window: can not create window error";
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	return window;
}

void opengl_init(vec4 bg_color) {
	glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
}

}

}