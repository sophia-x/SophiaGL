#include <camera/RotateCamera>

#include <scene/Scene>
#include <scene/WindowManager>
#include <tools/Timer>
using namespace gl;

#include <tools/callback_helper>

#include <sstream>
using namespace std;

#include <demo>

#define TITLE "Normal Mapping model demo"
#define RGB "RGB"

void normal_mapping_demo() {
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
	shared_ptr<StandardScene> scene_ptr = StandardScene::getScene(vec4(0, 0, WIDTH, HEIGHT), PointLight(vec3(0, 0, 4), vec3(1), 50.0f),
	                                      "shaders/NormalMapping.vertexshader", "shaders/NormalMapping.fragmentshader",
	vector<string> {
		"diffuse_texture",
		"normal_texture",
		"specular_texture",
		"MVP",
		"V",
		"M",
		"MV3x3",
		"light_position_worldspace",
		"light_color",
		"light_power",
		"material_ambient_color_ratio",
		"material_specular_color",
		"material_specular_ratio"
	});

	// Create Spirit
	shared_ptr<Spirit> spirit = Spirit::getImmortalSpirit();
	// Create Standard Model
	shared_ptr<NormalMappingModel> model_ptr;
	{
		model_ptr = NormalMappingModel::getModel("models/cylinder.obj", vector<pair<string, string>> {
			make_pair("Diffuse", "textures/diffuse.DDS"),
			make_pair("Normal", "textures/normal.bmp"),
			make_pair("Specular", "textures/specular.DDS")
		});
		// Create Material
		PhoneMaterial::addMaterial(RGB, vec3(0.1f), vec3(0.3f), 5);
		// Add ModelSpirit
		model_ptr->addSpirit(NormalMappingModelSpirit::getModelSpirit(spirit, model_ptr->getGLObj().getTexture("Diffuse"),
		                     model_ptr->getGLObj().getTexture("Normal"), model_ptr->getGLObj().getTexture("Specular"),
		                     PhoneMaterial::getMaterial(RGB)));
		// Add Model
		scene_ptr->addModel(RGB, model_ptr);
	}
	// Add Scene
	manager.addScene(WINDOW_NAME, scene_ptr);

	// Create DebugModel
	shared_ptr<PassthroughMvpToolModel> normal_ptr = PassthroughMvpToolModel::getTool(vec4(0, 0, WIDTH, HEIGHT), GL_LINES);
	normal_ptr->addSpirit(PassthroughMvpModelSpirit::getModelSpirit(spirit, vec4(1, 0, 0, 1)));

	shared_ptr<PassthroughMvpToolModel> tangent_ptr = PassthroughMvpToolModel::getTool(vec4(0, 0, WIDTH, HEIGHT), GL_LINES);
	tangent_ptr->addSpirit(PassthroughMvpModelSpirit::getModelSpirit(spirit, vec4(0, 1, 0, 1)));

	shared_ptr<PassthroughMvpToolModel> bitagent_ptr = PassthroughMvpToolModel::getTool(vec4(0, 0, WIDTH, HEIGHT), GL_LINES);
	bitagent_ptr->addSpirit(PassthroughMvpModelSpirit::getModelSpirit(spirit, vec4(0, 0, 1, 1)));

	size_t vertices_size, normals_size, tangents_size, bitangents_size;
	GLfloat* vertices = (GLfloat*)model_ptr->getData("Vertices", vertices_size);
	GLfloat* normals = (GLfloat*)model_ptr->getData("Normals", normals_size);
	GLfloat* tangents = (GLfloat*)model_ptr->getData("Tangents", tangents_size);
	GLfloat* bitangents = (GLfloat*)model_ptr->getData("Bitangents", bitangents_size);
	vector<vec3> normal_vertex, tangent_vertex, bitangent_vertex;
	normal_vertex.reserve((vertices_size / 3 * 2));
	tangent_vertex.reserve((vertices_size / 3 * 2));
	bitangent_vertex.reserve((vertices_size / 3 * 2));
	for (size_t i = 0; i < vertices_size; i += 3) {
		vec3 vertice(vertices[i], vertices[i + 1], vertices[i + 2]);
		normal_vertex.push_back(vertice);
		tangent_vertex.push_back(vertice);
		bitangent_vertex.push_back(vertice);

		normal_vertex.push_back(vertice + 0.1f * normalize(vec3(normals[i], normals[i + 1], normals[i + 2])));
		tangent_vertex.push_back(vertice + 0.1f * normalize(vec3(tangents[i], tangents[i + 1], tangents[i + 2])));
		bitangent_vertex.push_back(vertice + 0.1f * normalize(vec3(bitangents[i], bitangents[i + 1], bitangents[i + 2])));
	}
	normal_ptr->setData("Vertices", &normal_vertex[0], normal_vertex.size() * 3);
	tangent_ptr->setData("Vertices", &tangent_vertex[0], tangent_vertex.size() * 3);
	bitagent_ptr->setData("Vertices", &bitangent_vertex[0], bitangent_vertex.size() * 3);
	normal_ptr->bufferData(vector<string> {"Vertices"});
	tangent_ptr->bufferData(vector<string> {"Vertices"});
	bitagent_ptr->bufferData(vector<string> {"Vertices"});
	manager.addTool("NormalDebug", normal_ptr);
	manager.addTool("TangentDebug", tangent_ptr);
	manager.addTool("BitangentDebug", bitagent_ptr);

	// Create TextTool
	shared_ptr<TextToolModel> text_ptr = TextToolModel::getTool(vec4(0, 0, WIDTH, HEIGHT), vec2(800, 600));
	manager.addTool("Text", text_ptr);

	Timer timer;
	while (manager.next()) {
		timer.tick();
		double delta = timer.getDelta();

		ostringstream oss;
		oss << timer.getFrameRate() << " frames/s";
		text_ptr->clear();
		text_ptr->print_text(oss.str(), 10, 500, 60);

		manager.step(delta, vector<string> {"NormalDebug", "TangentDebug", "BitangentDebug", "Text"});
	}

}