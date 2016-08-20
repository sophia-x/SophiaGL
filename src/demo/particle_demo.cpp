#include <camera/RotateCamera>

#include <scene/Scene>
#include <scene/WindowManager>
#include <tools/Timer>
using namespace gl;

#include <tools/callback_helper>

#include <glm/gtc/random.hpp>

#include <demo>

#define TITLE "Particle model demo"
#define RGB "RGB"

static void addRandomParticles(size_t num, shared_ptr<ParticleToolModel> &model_ptr) {
	for (size_t i = 0; i < num; i ++) {
		model_ptr->addSpirit(ParticleModelSpirit::getModelSpirit(ParticleSpirit::getMortalSpirit(5.0f, vec3(0, 0, -20.0f), vec3(0.0f, 10.0f, 0.0f)
		                     + ballRand(1.5f), vec3(0.0f, -9.81f, 0.0f)), linearRand(vec4(0.0f), vec4(vec3(1.0f), 1.0f / 3))));
	}
}

void particle_demo() {
	// Get Window Manager
	WindowManager &manager = WindowManager::getWindowManager();

	// Add Window
	manager.addWindow(WINDOW_NAME, Helper::createWindow(WIDTH, HEIGHT, TITLE));
	manager.setCurrent(WINDOW_NAME);
	// Turn glew & debug on
	manager.turnGlewOn();
	manager.turnDebugOn(CallBackHelper::debugOutputCallback);
	// Add camera
	manager.pushCamera(RotateCamera::getCamera(vec3(0, 0, 5), 1.0f, 0.005f, 0.005f));
	// Init opengl
	Helper::opengl_init(BG_COLOR);

	// Create Standard Model
	shared_ptr<ParticleToolModel> model_ptr = ParticleToolModel::getTool(vec4(0, 0, WIDTH, HEIGHT), GL_TRIANGLE_STRIP);
	vector<GLfloat> data = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
	};
	model_ptr->setData("Vertices", &data[0], data.size());
	model_ptr->bufferData(vector<string> {"Vertices"});

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		{
			static float NUM_PER_SECOND = 10000.0;
			int num = (int)clamp(delta * NUM_PER_SECOND, 0.0, 0.016 * NUM_PER_SECOND);
			addRandomParticles(num, model_ptr);
		}

		manager.step(delta);

		model_ptr->update(delta);
		model_ptr->draw();
	}
}