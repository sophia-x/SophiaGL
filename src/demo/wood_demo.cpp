#include <camera/RotateCamera>

#include <scene/Scene>
#include <scene/WindowManager>
#include <tools/Timer>
using namespace gl;

#include <tools/callback_helper>

#include <demo>

#define TITLE "Wood model demo"
#define RGB "RGB"

void wood_demo() {
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
	shared_ptr<RenderTextureScene> scene_ptr = RenderTextureScene::getScene(vec4(0, 0, WIDTH, HEIGHT), PointLight(vec3(0.5, 2, 2), vec3(1), 50.0f));
	// Create Standard Model
	shared_ptr<StandardModel> model_ptr = StandardModel::initModel("models/monkey.obj", vector<pair<string, string>> {
		make_pair(RGB, "textures/monkey.DDS")
	});
	// Create Material
	PhoneMaterial::addMaterial(RGB, vec3(0.1f), vec3(0.3f), 5);
	// Add Model
	scene_ptr->addModel(model_ptr->getInstance(Spirit::getImmortalSpirit(), model_ptr->getGLObj().getTexture(RGB),
	                    PhoneMaterial::getMaterial(RGB)));
	// Add Scene
	manager.addScene(WINDOW_NAME, scene_ptr);

	shared_ptr<VertexTextureToolModel> cube_ptr = VertexTextureToolModel::initTool(vec4(0, 0, WIDTH, HEIGHT), GL_TRIANGLES, scene_ptr->getTexture(0),
	"shaders/Passthrough.vertexshader", "shaders/WobblyTexture.fragmentshader", vector<string> {
		"time",
		"texture_in",
		"M"
	});
	cube_ptr->setSetter(TimeSetter::getSetter(Setter::getSetter(), cube_ptr->getShaderPtr()->getUniform("time")));
	vector<GLfloat> data = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,
	};
	vector<GLfloat> uvs(data.size() / 3 * 2);
	for (size_t i = 0; i < uvs.size() / 2; i++) {
		uvs[2 * i] = (data[3 * i] + 1) / 2;
		uvs[2 * i + 1] = (data[3 * i + 1] + 1) / 2;
	}
	cube_ptr->setData("Vertices", &data[0], data.size());
	cube_ptr->setData("Uvs", &uvs[0], uvs.size());
	cube_ptr->bufferData(vector<string> {"Vertices", "Uvs"});
	cube_ptr = cube_ptr->getInstance(Spirit::getImmortalSpirit());

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		manager.step(delta, vector<string> {WINDOW_NAME});

		cube_ptr->draw();
	}

}