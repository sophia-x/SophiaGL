#include <camera/RotateCamera>

#include <scene/Scene>
#include <scene/WindowManager>
#include <tools/Timer>
using namespace gl;

#include <tools/callback_helper>

#include <demo>

#define TITLE "Shadow map model demo"
#define RGB "RGB"

void shadow_map_demo() {
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
	shared_ptr<ShadowMapScene> scene_ptr = ShadowMapScene::getScene(vec4(0, 0, WIDTH, HEIGHT), DirectionLight(vec3(0.5, 2, 2), vec3(1), 1.0f));
	// Create Standard Model
	shared_ptr<StandardModel> model_ptr = StandardModel::initModel("models/room_thickwalls.obj", vector<pair<string, string>> {
		make_pair(RGB, "textures/room_thickwalls.DDS")
	},
	"shaders/ShadowMapping.vertexshader", "shaders/ShadowMapping.fragmentshader",
	vector<string> {
		"MVP",
		"V",
		"M",
		"depth_bias_MVP",
		"diffuse_texture",
		"light_direction_worldspace",
		"light_color",
		"light_power",
		"material_ambient_color_ratio",
		"material_specular_color",
		"material_specular_ratio",
		"bias_ratio",
		"shadowMap"
	});
	// Create Material
	PhoneMaterial::addMaterial(RGB, vec3(0.1f), vec3(0.3f), 5);
	// Add Model
	scene_ptr->addModel(model_ptr->getInstance(Spirit::getImmortalSpirit(), model_ptr->getGLObj().getTexture(RGB),
	                    PhoneMaterial::getMaterial(RGB)));
	// Add Scene
	manager.addScene(WINDOW_NAME, scene_ptr);

	shared_ptr<VertexTextureToolModel> cube_ptr = VertexTextureToolModel::initTool(vec4(0, 0, 256, 256), GL_TRIANGLES, scene_ptr->getTexture(0));
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