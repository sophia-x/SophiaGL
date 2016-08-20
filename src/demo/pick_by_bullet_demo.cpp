#include <sstream>

#include <camera/RotateCamera>

#include <scene/Scene>
#include <scene/WindowManager>
#include <tools/Timer>
using namespace gl;

#include <tools/callback_helper>

#include <btBulletDynamicsCommon.h>

#include <AntTweakBar.h>

#include <demo>

#define TITLE "Pick by bullet model demo"
#define RGB "RGB"

class BulletDebugDrawer_OpenGL : public btIDebugDraw {
public:
	BulletDebugDrawer_OpenGL(): debug_ptr{PassthroughMvpToolModel::initTool(vec4(0, 0, WIDTH, HEIGHT), GL_LINES, vec4(1, 0, 0, 1))
		                                      ->getInstance(Spirit::getImmortalSpirit())} {}

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
		vec3 line[2];
		line[0] = vec3(from.x(), from.y(), from.z());
		line[1] = vec3(to.x(), to.y(), to.z());

		debug_ptr->setData("Vertices", &line[0], 6);
		debug_ptr->bufferData(vector<string> {"Vertices"});

		debug_ptr->setColor(vec4(color.x(), color.y(), color.z(), 1));

		debug_ptr->draw();
	}

	virtual void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) {}

	virtual void reportErrorWarning(const char *) {}

	virtual void draw3dText(const btVector3 &, const char *) {}

	virtual void setDebugMode(int p) {
		m = p;
	}

	int getDebugMode(void) const {
		return 3;
	}

private:
	int m;
	shared_ptr<PassthroughMvpToolModel> debug_ptr;
};

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

btDiscreteDynamicsWorld* dynamics_world;
btCollisionShape* box_collision_shape;

class BulletListener: public AddModelListener {
public:
	void modelAdded(const shared_ptr<Model>& model, size_t i) {
		const quat& origentation = model->spirit_ptr()->getOrig();
		const vec3& pos = model->spirit_ptr()->getPos();
		btDefaultMotionState* motionstate = new btDefaultMotionState(btTransform(
		            btQuaternion(origentation.x, origentation.y, origentation.z, origentation.w),
		            btVector3(pos.x, pos.y, pos.z)
		        ));
		btRigidBody::btRigidBodyConstructionInfo rigid_body_CI(
		    0,                  // mass, in kg. 0 -> Static object, will never move.
		    motionstate,
		    box_collision_shape,  // collision shape of body
		    btVector3(0, 0, 0)  // local inertia
		);
		btRigidBody *rigidBody = new btRigidBody(rigid_body_CI);
		dynamics_world->addRigidBody(rigidBody);
		rigidBody->setUserPointer((void*)i);
	}
};

void pick_by_bullet_demo() {
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

	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collision_configuration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collision_configuration);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	dynamics_world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -9.81f, 0));
	box_collision_shape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));

	BulletDebugDrawer_OpenGL mydebugdrawer;
	dynamics_world->setDebugDrawer(&mydebugdrawer);

	// Create Standard Scene
	shared_ptr<StandardScene> scene_ptr = StandardScene::getScene(vec4(0, 0, WIDTH, HEIGHT), PointLight(vec3(4), vec3(1), 50.0f));
	scene_ptr->addListener(new BulletListener());
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

		dynamics_world->stepSimulation(delta, 7);

		if (glfwGetMouseButton(WindowManager::getWindowManager().getCurrentWindow(), GLFW_MOUSE_BUTTON_LEFT)) {
			vec3 out_origin;
			vec3 out_direction;
			const Camera& camera = *WindowManager::getWindowManager().currentCamera();
			const mat4 &projection_matrix = camera.getProjectionMatrix();
			const mat4 &view_matrix = camera.getViewMatrix();
			screenPosToWorldRay(
			    WIDTH / 2, HEIGHT / 2,
			    WIDTH, HEIGHT,
			    view_matrix,
			    projection_matrix,
			    out_origin,
			    out_direction
			);
			vec3 out_end = out_origin + out_direction * 1000.0f;

			btCollisionWorld::ClosestRayResultCallback ray_callback(btVector3(out_origin.x, out_origin.y, out_origin.z),
			        btVector3(out_end.x, out_end.y, out_end.z));
			dynamics_world->rayTest(btVector3(out_origin.x, out_origin.y, out_origin.z),
			                        btVector3(out_end.x, out_end.y, out_end.z), ray_callback);
			if (ray_callback.hasHit()) {
				std::ostringstream oss;
				oss << "mesh " << (size_t)ray_callback.m_collisionObject->getUserPointer();
				message = oss.str();
			} else {
				message = "background";
			}
		}

		dynamics_world->debugDrawWorld();

		manager.step(delta, vector<string> {WINDOW_NAME});

		TwDraw();
	}

	TwTerminate();
}