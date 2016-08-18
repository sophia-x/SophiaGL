#include <sstream>

#include <camera/RotateCamera>

#include <scene/Scene>
#include <scene/WindowManager>
#include <tools/Timer>
using namespace gl;

#include <tools/callback_helper>

#include <AntTweakBar.h>

#include <demo>

#define TITLE "Pick by color model demo"
#define RGB "RGB"

static string message;
static void setTwUI() {
	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WIDTH, HEIGHT);
	TwBar * GUI = TwNewBar("Picking");
	TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &message, NULL);
}

void pick_by_color_demo() {
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

	setTwUI();

	// Create Standard Scene
	shared_ptr<Scene> scene_ptr = StandardScene::getScene(vec4(0, 0, WIDTH, HEIGHT), PointLight(vec3(4), vec3(1), 50.0f));
	// Create Standard Model
	shared_ptr<Model> model_ptr = StandardModel::getModel("models/monkey.obj", vector<pair<string, string>> {
		make_pair(RGB, "textures/monkey.DDS")
	});
	// Create Material
	PhoneMaterial::addMaterial(RGB, vec3(0.1f), vec3(0.3f), 5);
	// Add ModelSpirit
	for (int i = 0; i < 100; i++) {
		vec3 pos = vec3(rand() % 20 - 10, rand() % 20 - 10, rand() % 20 - 10);
		quat orientation = quat(vec3(rand() % 360, rand() % 360, rand() % 360));
		model_ptr->addSpirit(StandardModelSpirit::getModelSpirit(Spirit::getImmortalSpirit(pos, orientation), model_ptr->getGLObj().getTexture(RGB),
		                     PhoneMaterial::getMaterial(RGB)));
	}

	// Add Model
	scene_ptr->addModel(RGB, model_ptr);
	// Add Scene
	manager.addScene(WINDOW_NAME, scene_ptr);

	shared_ptr<PassthroughMvpToolModel> debug_ptr = PassthroughMvpToolModel::getTool(vec4(0, 0, WIDTH, HEIGHT), GL_TRIANGLES, true);
	debug_ptr->setGLObj(model_ptr->getGLObjPtr());
	const vector<shared_ptr<BaseModelSpirit>>& spirits = model_ptr->getSpirits();
	for (size_t i = 0; i < spirits.size(); i++) {
		int r = (i & 0x000000FF) >>  0;
		int g = (i & 0x0000FF00) >>  8;
		int b = (i & 0x00FF0000) >> 16;

		debug_ptr->addSpirit(PassthroughMvpModelSpirit::getModelSpirit(spirits[i]->getSpiritPtr(), vec3(r / 255.0f, g / 255.0f, b / 255.0f)));
	}

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		if (glfwGetMouseButton(WindowManager::getWindowManager().getCurrentWindow(), GLFW_MOUSE_BUTTON_LEFT)) {
			static GLfloat color[4];
			glGetFloatv(GL_COLOR_CLEAR_VALUE, color);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			debug_ptr->draw();

			glFlush();
			glFinish();
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			unsigned char data[4];
			glReadPixels(WIDTH / 2, HEIGHT / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

			int pickedID =
			    data[0] +
			    data[1] * 256 +
			    data[2] * 256 * 256;

			if (pickedID == 0x00ffffff) {
				message = "background";
			} else {
				std::ostringstream oss;
				oss << "mesh " << pickedID;
				message = oss.str();
			}

			glClearColor(color[0], color[1], color[2], color[3]);
		}

		manager.step(delta);

		TwDraw();
	}

	TwTerminate();
}