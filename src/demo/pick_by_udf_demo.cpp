#include <sstream>

#include <camera/RotateCamera>

#include <scene/Scene>
#include <scene/WindowManager>
#include <tools/Timer>
using namespace gl;

#include <tools/callback_helper>

#include <AntTweakBar.h>

#include <demo>

#define TITLE "Pick by user define model demo"
#define RGB "RGB"

static bool testRayOBBIntersection(
    vec3 ray_origin,
    vec3 ray_direction,
    vec3 aabb_min,
    vec3 aabb_max,
    mat4 model_matrix,
    float& intersection_distance
) {
	float t_min = 0.0f;
	float t_max = 100000.0f;

	vec3 OBB_position_worldspace(model_matrix[3].x, model_matrix[3].y, model_matrix[3].z);

	vec3 delta = OBB_position_worldspace - ray_origin;
	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
		vec3 xaxis(model_matrix[0].x, model_matrix[0].y, model_matrix[0].z);
		float e = dot(xaxis, delta);
		float f = dot(ray_direction, xaxis);

		if ( fabs(f) > 0.001f ) { // Standard case
			float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
			float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
			if (t1 > t2) {
				float w = t1; t1 = t2; t2 = w; // swap t1 and t2
			}

			// t_max is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if ( t2 < t_max )
				t_max = t2;
			// t_min is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if ( t1 > t_min )
				t_min = t1;
			if (t_max < t_min )
				return false;
		} else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
				return false;
		}
	}

	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	{
		vec3 yaxis(model_matrix[1].x, model_matrix[1].y, model_matrix[1].z);
		float e = dot(yaxis, delta);
		float f = dot(ray_direction, yaxis);

		if ( fabs(f) > 0.001f ) {

			float t1 = (e + aabb_min.y) / f;
			float t2 = (e + aabb_max.y) / f;

			if (t1 > t2) {float w = t1; t1 = t2; t2 = w;}

			if ( t2 < t_max )
				t_max = t2;
			if ( t1 > t_min )
				t_min = t1;
			if (t_min > t_max)
				return false;

		} else {
			if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
				return false;
		}
	}

	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	{
		vec3 zaxis(model_matrix[2].x, model_matrix[2].y, model_matrix[2].z);
		float e = dot(zaxis, delta);
		float f = dot(ray_direction, zaxis);

		if ( fabs(f) > 0.001f ) {

			float t1 = (e + aabb_min.z) / f;
			float t2 = (e + aabb_max.z) / f;

			if (t1 > t2) {float w = t1; t1 = t2; t2 = w;}

			if ( t2 < t_max )
				t_max = t2;
			if ( t1 > t_min )
				t_min = t1;
			if (t_min > t_max)
				return false;

		} else {
			if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
				return false;
		}
	}

	intersection_distance = t_min;
	return true;
}

static string message;
static void setTwUI() {
	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WIDTH, HEIGHT);
	TwBar * GUI = TwNewBar("Picking");
	TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &message, NULL);
}

static void screenPosToWorldRay(
    int mouse_x, int mouse_y, int screen_width, int screen_height, mat4 view_matrix, mat4 projection_matrix,
    vec3& out_origin, vec3& out_direction
) {
	vec4 ray_start_NDC(
	    ((float)mouse_x / (float)screen_width  - 0.5f) * 2.0f,
	    ((float)mouse_y / (float)screen_height - 0.5f) * 2.0f,
	    -1.0,
	    1.0f
	);
	vec4 ray_end_NDC(
	    ((float)mouse_x / (float)screen_width  - 0.5f) * 2.0f,
	    ((float)mouse_y / (float)screen_height - 0.5f) * 2.0f,
	    0.0,
	    1.0f
	);

	mat4 M = inverse(projection_matrix * view_matrix);
	vec4 ray_start_world = M * ray_start_NDC; ray_start_world /= ray_start_world.w;
	vec4 ray_end_world   = M * ray_end_NDC  ; ray_end_world  /= ray_end_world.w;

	vec3 ray_dir_world(ray_end_world - ray_start_world);
	ray_dir_world = normalize(ray_dir_world);

	out_origin = vec3(ray_start_world);
	out_direction = normalize(ray_dir_world);
}

static vector<shared_ptr<Spirit>> spirits;

class UdfListener: public AddModelListener {
public:
	void modelAdded(const shared_ptr<Model>& model, size_t i) {
		spirits.push_back(model->spirit_ptr());
	}
};

void pick_by_udf_demo() {
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
	shared_ptr<StandardScene> scene_ptr = StandardScene::getScene(vec4(0, 0, WIDTH, HEIGHT), PointLight(vec3(4), vec3(1), 50.0f));
	scene_ptr->addListener(new UdfListener);
	// Create Standard Model
	shared_ptr<StandardModel> model_ptr = StandardModel::initModel("models/monkey.obj", vector<pair<string, string>> {
		make_pair(RGB, "textures/monkey.DDS")
	});
	// Create Material
	PhoneMaterial::addMaterial(RGB, vec3(0.1f), vec3(0.3f), 5);
	// Add ModelSpirit
	for (int i = 0; i < 100; i++) {
		vec3 pos = vec3(rand() % 20 - 10, rand() % 20 - 10, rand() % 20 - 10);
		quat orientation = quat(vec3(rand() % 360, rand() % 360, rand() % 360));
		scene_ptr->addModel(model_ptr->getInstance(Spirit::getImmortalSpirit(pos, orientation), model_ptr->getGLObj().getTexture(RGB),
		                    PhoneMaterial::getMaterial(RGB)));
	}

	// Add Scene
	manager.addScene(WINDOW_NAME, scene_ptr);

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		if (glfwGetMouseButton(WindowManager::getWindowManager().getCurrentWindow(), GLFW_MOUSE_BUTTON_LEFT)) {
			vec3 ray_origin;
			vec3 ray_direction;
			const Camera& camera = *WindowManager::getWindowManager().currentCamera();
			const mat4 &projection_matrix = camera.getProjectionMatrix();
			const mat4 &view_matrix = camera.getViewMatrix();
			screenPosToWorldRay(
			    WIDTH / 2, HEIGHT / 2,
			    WIDTH, HEIGHT,
			    view_matrix,
			    projection_matrix,
			    ray_origin,
			    ray_direction
			);

			message = "background";
			float min_dist = -1;
			size_t id = -1;
			for (size_t i = 0; i < spirits.size(); i++) {
				const Spirit& spirit = *spirits[i];

				float intersection_distance;
				vec3 aabb_min(-1.0f, -1.0f, -1.0f);
				vec3 aabb_max( 1.0f,  1.0f,  1.0f);

				if ( testRayOBBIntersection(
				            ray_origin,
				            ray_direction,
				            aabb_min,
				            aabb_max,
				            spirit.getModelMatrix(),
				            intersection_distance)
				   ) {
					if (min_dist < 0 || intersection_distance < min_dist) {
						min_dist = intersection_distance;
						id = i;
						break;
					}
				}
			}
			if (id != -1) {
				std::ostringstream oss;
				oss << "mesh " << id;
				message = oss.str();
			}
		}

		manager.step(delta, vector<string> {WINDOW_NAME});

		TwDraw();
	}

	TwTerminate();
}

