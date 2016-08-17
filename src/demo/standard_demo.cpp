#include <camera/RotateCamera>

#include <scene/Scene>
#include <scene/WindowManager>
#include <tools/Timer>
using namespace gl;

#include <tools/callback_helper>

#include <demo>

#define TITLE "Standard model demo"
#define RGB "RGB"

void standard_demo() {
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

	// Create Standard Scene
	shared_ptr<Scene> scene_ptr = StandardScene::getScene(vec4(0, 0, WIDTH, HEIGHT), PointLight(vec3(4), vec3(1), 50.0f));
	// Create Standard Model
	shared_ptr<Model> model_ptr = StandardModel::getModel("models/monkey.obj", vector<pair<string, string>> {
		make_pair(RGB, "textures/monkey.DDS")
	});
	// Create Material
	PhoneMaterial::addMaterial(RGB, vec3(0.1f), vec3(0.3f), 5);
	// Add ModelSpirit
	model_ptr->addSpirit(StandardModelSpirit::getModelSpirit(Spirit::getImmortalSpirit(), model_ptr->getGLObj().getTexture(RGB),
	                     PhoneMaterial::getMaterial(RGB)));
	// Add Model
	scene_ptr->addModel(RGB, model_ptr);
	// Add Scene
	manager.addScene(WINDOW_NAME, scene_ptr);


	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		manager.step(delta);
	}

}