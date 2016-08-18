#include <camera/RotateCamera>

#include <scene/Scene>
#include <scene/WindowManager>
#include <tools/Timer>
using namespace gl;

#include <tools/callback_helper>

#include <demo>

#define TITLE "Billboard 2D demo"
#define RGB "RGB"

void billboard_demo() {
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

	// Create DebugModel
	shared_ptr<VertexColorToolModel> cube_ptr = VertexColorToolModel::getTool(vec4(0, 0, WIDTH, HEIGHT), GL_TRIANGLES);
	cube_ptr->addSpirit(BaseModelSpirit::getModelSpirit(Spirit::getImmortalSpirit(vec3(), quat(), vec3(0.2f))));
	vector<GLfloat> vertex_data = { -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f};
	vector<GLfloat> color_data = { 0.583f,  0.771f,  0.014f, 0.609f,  0.115f,  0.436f, 0.327f,  0.483f,  0.844f, 0.822f,  0.569f,  0.201f, 0.435f,  0.602f,  0.223f, 0.310f,  0.747f,  0.185f, 0.597f,  0.770f,  0.761f, 0.559f,  0.436f,  0.730f, 0.359f,  0.583f,  0.152f, 0.483f,  0.596f,  0.789f, 0.559f,  0.861f,  0.639f, 0.195f,  0.548f,  0.859f, 0.014f,  0.184f,  0.576f, 0.771f,  0.328f,  0.970f, 0.406f,  0.615f,  0.116f, 0.676f,  0.977f,  0.133f, 0.971f,  0.572f,  0.833f, 0.140f,  0.616f,  0.489f, 0.997f,  0.513f,  0.064f, 0.945f,  0.719f,  0.592f, 0.543f,  0.021f,  0.978f, 0.279f,  0.317f,  0.505f, 0.167f,  0.620f,  0.077f, 0.347f,  0.857f,  0.137f, 0.055f,  0.953f,  0.042f, 0.714f,  0.505f,  0.345f, 0.783f,  0.290f,  0.734f, 0.722f,  0.645f,  0.174f, 0.302f,  0.455f,  0.848f, 0.225f,  0.587f,  0.040f, 0.517f,  0.713f,  0.338f, 0.053f,  0.959f,  0.120f, 0.393f,  0.621f,  0.362f, 0.673f,  0.211f,  0.457f, 0.820f,  0.883f,  0.371f, 0.982f,  0.099f,  0.879f};
	cube_ptr->setData("Vertices", &vertex_data[0], vertex_data.size());
	cube_ptr->setData("Color", &color_data[0], color_data.size());
	cube_ptr->bufferData(vector<string> {"Vertices", "Color"});
	manager.addTool("CUBE", cube_ptr);

	shared_ptr<BillboardToolModel> billboard_ptr = BillboardToolModel::getTool(vec4(0, 0, WIDTH, HEIGHT), vec4(0.04, 0.3, 1, 0.7),
	        vec4(0.2, 0.8, 0.2, 1.0), GL_TRIANGLE_STRIP, "textures/Billboard.DDS", "shaders/Billboard_fix.vertexshader");
	shared_ptr<Spirit> base_ptr = Spirit::getImmortalSpirit(vec3(0, 0.5, 0), quat(), vec3(1, 0.125, 0));
	billboard_ptr->addSpirit(BaseModelSpirit::getModelSpirit(base_ptr));
	vector<GLfloat> data = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
	};
	vector<GLfloat> uvs(data.size() / 3 * 2);
	for (size_t i = 0; i < uvs.size() / 2; i ++) {
		uvs[2 * i] = data[3 * i] + 0.5;
		uvs[2 * i + 1] = data[3 * i + 1] + 0.5;
	}
	billboard_ptr->setData("Vertices", &data[0], data.size());
	billboard_ptr->setData("Uvs", &uvs[0], uvs.size());
	billboard_ptr->bufferData(vector<string> {"Vertices", "Uvs"});
	manager.addTool("Billboard", billboard_ptr);

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		float life_level = sin(timer.getCurrentTime()) * 0.1f + 0.7f;
		base_ptr->setLife(life_level);

		manager.step(delta, vector<string> {"CUBE", "Billboard"});
	}

}