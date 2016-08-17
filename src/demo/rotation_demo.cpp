#include <camera/Camera>

#include <scene/Scene>
#include <scene/WindowManager>

#include <tools/Timer>
using namespace gl;

#include <tools/quaternion>
#include <tools/callback_helper>

#include <demo>

#include <AntTweakBar.h>

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace glm;

#define TITLE "Rotation demo"
#define RGB "RGB"

#define QUATERN_LOC "808 16"

static vec3 euler_pos(-1.5f, 0.0f, 0.0f);
static vec3 euler_orig;
static vec3 quat_pos( 1.5f, 0.0f, 0.0f);
static quat quat_orig;
static bool face_to = true;

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	TwEventMouseButtonGLFW(button, action);
}
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	TwEventMousePosGLFW(xpos, ypos);
}

static void setTwUI() {
	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WIDTH, HEIGHT);
	TwBar *EulerGUI = TwNewBar("Euler settings");
	TwBar *QuaternionGUI = TwNewBar("Quaternion settings");
	TwSetParam(EulerGUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwSetParam(QuaternionGUI, NULL, "position", TW_PARAM_CSTRING, 1, QUATERN_LOC);

	TwAddVarRW(EulerGUI, "Euler X", TW_TYPE_FLOAT, &euler_orig.x, "step=0.01");
	TwAddVarRW(EulerGUI, "Euler Y", TW_TYPE_FLOAT, &euler_orig.y, "step=0.01");
	TwAddVarRW(EulerGUI, "Euler Z", TW_TYPE_FLOAT, &euler_orig.z, "step=0.01");
	TwAddVarRW(EulerGUI, "Pos X"  , TW_TYPE_FLOAT, &euler_pos.x, "step=0.1");
	TwAddVarRW(EulerGUI, "Pos Y"  , TW_TYPE_FLOAT, &euler_pos.y, "step=0.1");
	TwAddVarRW(EulerGUI, "Pos Z"  , TW_TYPE_FLOAT, &euler_pos.z, "step=0.1");

	TwAddVarRW(QuaternionGUI, "Quaternion", TW_TYPE_QUAT4F, &quat_orig, "showval=true  opened=true");
	TwAddVarRW(QuaternionGUI, "Face to", TW_TYPE_BOOL8 , &face_to, "help='Face to the other monkey ?'");

	glfwSetMouseButtonCallback(WindowManager::getWindowManager().getCurrentWindow(), mouse_button_callback);
	glfwSetCursorPosCallback(WindowManager::getWindowManager().getCurrentWindow(), cursor_position_callback);
}

void rotation_demo() {
	// Get Window Manager
	WindowManager &manager = WindowManager::getWindowManager();

	// Add Window
	manager.addWindow(WINDOW_NAME, Helper::createWindow(WIDTH, HEIGHT, TITLE));
	manager.setCurrent(WINDOW_NAME);
	// Turn glew & debug on
	manager.turnGlewOn();
	manager.turnDebugOn(CallBackHelper::debugOutputCallback);
	// Set up UI
	setTwUI();
	// Add camera
	manager.pushCamera(StaticCamera::getCamera(vec3(0, 0, 7)));
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
	shared_ptr<StandardModelSpirit> euler_spirit_ptr = StandardModelSpirit::getModelSpirit(Spirit::getImmortalSpirit(), model_ptr->getGLObj().getTexture(RGB),
	        PhoneMaterial::getMaterial(RGB));
	shared_ptr<StandardModelSpirit> quat_spirit_ptr = StandardModelSpirit::getModelSpirit(Spirit::getImmortalSpirit(), model_ptr->getGLObj().getTexture(RGB),
	        PhoneMaterial::getMaterial(RGB));
	model_ptr->addSpirit(euler_spirit_ptr);
	model_ptr->addSpirit(quat_spirit_ptr);
	// Add Model
	scene_ptr->addModel(RGB, model_ptr);
	// Add Scene
	manager.addScene(WINDOW_NAME, scene_ptr);

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		{
			euler_spirit_ptr->spirit().setPos(euler_pos);
			quat_spirit_ptr->spirit().setPos(quat_pos);

			euler_orig.y += 3.14159f / 2.0f * delta;
			euler_spirit_ptr->spirit().setOrig(toQuat(eulerAngleYXZ(euler_orig.y, euler_orig.x, euler_orig.z)));

			if (face_to) {
				vec3 dir = euler_pos - quat_pos;
				vec3 up = Quat::Y;
				// Compute the desired orientation
				quat orig = normalize(Quat::lookAt(dir, up));
				// And interpolate
				quat_orig = Quat::rotateTowards(quat_orig, orig, 1.0f * delta);
			}
			quat_spirit_ptr->spirit().setOrig(quat_orig);
		}

		manager.step(delta);

		TwDraw();
	}

	TwTerminate();
}