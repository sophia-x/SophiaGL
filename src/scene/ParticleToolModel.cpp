#include <camera/Camera>

#include <scene/Model>
#include <scene/WindowManager>

namespace gl
{

static bool life_compare_heap(const shared_ptr<ParticleSpirit>& a, const shared_ptr<ParticleSpirit>& b) {
	return a->getLife() > b->getLife();
}

static bool dist_compare(const shared_ptr<ParticleSpirit>& a, const shared_ptr<ParticleSpirit>& b) {
	const Camera& camera = *WindowManager::getWindowManager().currentCamera();
	const vec3& camera_pos = camera.getPosition();

	return distance(camera_pos, a->getPos()) > distance(camera_pos, b->getPos());
}

shared_ptr<ParticleToolModel> ParticleToolModel::initTool(const vec4& p_border, GLenum p_mode,
        const string& p_texture, const string &vertex_path, const string &fragment_path, const vector<string>& uniforms) {
	// Create GLObj
	shared_ptr<GLObj> obj_ptr = GLObj::getGLObj();
	{
		// Add GLBuffer
		obj_ptr->addBuffer("Vertices", GLDataObj::getGLBuffer((void*)0, 0, 3, GL_STATIC_DRAW, 0));
		obj_ptr->addBuffer("xyzs", GLStreamObj::getGLBuffer((void*)0, 0, 4, GL_STREAM_DRAW, 1));
		obj_ptr->addBuffer("colors", GLStreamObj::getGLBuffer((void*)0, 0, 4, GL_STREAM_DRAW, 1));

		obj_ptr->addDrawObj("Stream", GLInstanceDraw::getDrawObj(obj_ptr->getBuffer("Vertices"), p_mode));
		obj_ptr->addTexture("Particle", p_texture);

		return shared_ptr<ParticleToolModel>(new ParticleToolModel(obj_ptr, GLShader::getShader(vertex_path,
		                                     fragment_path, uniforms), p_border, vector<string> {"Vertices", "xyzs", "colors"}, "Stream"));
	}
}

void ParticleToolModel::sortByLife() {
	push_heap(spirits.begin(), spirits.end(), life_compare_heap);
}

void ParticleToolModel::update(float delta) {
	Model::update(delta);

	sortByLife();

	while (!spirits.empty() && !spirits[0]->alive()) {
		pop_heap(spirits.begin(), spirits.end(), life_compare_heap);
		spirits.pop_back();
	}

	vector<shared_ptr<ParticleSpirit>> vec = spirits;

	sort(vec.begin(), vec.end(), dist_compare);

	vector<GLfloat> positions(4 * spirits.size());
	vector<GLfloat> colors(4 * spirits.size());
	for (size_t i = 0; i < vec.size(); i ++) {
		const ParticleSpirit& sp = *vec[i];
		positions[4 * i + 0] = sp.getPos().x;
		positions[4 * i + 1] = sp.getPos().y;
		positions[4 * i + 2] = sp.getPos().z;
		positions[4 * i + 3] = sp.getSize().x;

		colors[4 * i + 0] = sp.getColor().x;
		colors[4 * i + 1] = sp.getColor().y;
		colors[4 * i + 2] = sp.getColor().z;
		colors[4 * i + 3] = sp.getColor().w;
	}

	gl_obj->setData("xyzs", &positions[0], positions.size());
	gl_obj->setData("colors", &colors[0], colors.size());
	gl_obj->bufferData(vector<string> {"xyzs", "colors"});

	const shared_ptr<GLInstanceDraw>& draw_ptr = (const shared_ptr<GLInstanceDraw>&)gl_obj->getDrawObj("Stream");
	draw_ptr->setCount(spirits.size());
}

void ParticleToolModel::draw() const {
	glViewport(border[0], border[1], border[2], border[3]);
	shader_ptr->useShader();
	{
		setter->setup();
		const Camera& camera = *WindowManager::getWindowManager().currentCamera();
		const mat4 &projection_matrix = camera.getProjectionMatrix();
		const mat4 &view_matrix = camera.getViewMatrix();
		mat4 VP = projection_matrix * view_matrix;

		glUniform3f(shader_ptr->getUniform("camera_right_worldspace"), view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
		glUniform3f(shader_ptr->getUniform("camera_up_worldspace"), view_matrix[0][1], view_matrix[1][1], view_matrix[2][1]);
		glUniformMatrix4fv(shader_ptr->getUniform("vp"), 1, GL_FALSE, &VP[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gl_obj->getTexture("Particle"));
		glUniform1i(shader_ptr->getUniform("texture_in"), 0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		gl_obj->draw(draw_vec, draw_obj);

		glDisable(GL_BLEND);
	}
	shader_ptr->unuseShader();
}

}