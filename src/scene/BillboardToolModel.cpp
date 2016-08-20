#include <camera/Camera>

#include <scene/Model>
#include <scene/WindowManager>

namespace gl
{

shared_ptr<BillboardToolModel> BillboardToolModel::getTool(const vec4& p_border, const vec4& p_board_border, const vec4& p_life_color, GLenum p_mode,
        const string& p_texture, const string &vertex_path, const string &fragment_path, const vector<string>& uniforms) {
	// Create GLObj
	shared_ptr<GLObj> obj_ptr = GLObj::getGLObj();
	{
		// Add GLBuffer
		obj_ptr->addBuffer("Vertices", GLDataObj::getGLBuffer((void*)0, 0, 3));
		obj_ptr->addBuffer("Uvs", GLDataObj::getGLBuffer((void*)0, 0, 2));

		obj_ptr->addDrawObj("Array", GLArrayDraw::getDrawObj(obj_ptr->getBuffer("Vertices"), p_mode));

		obj_ptr->addTexture("Billboard", p_texture);

		return shared_ptr<BillboardToolModel>(new BillboardToolModel(obj_ptr, GLShader::getShader(vertex_path,
		                                      fragment_path, uniforms), p_border, p_board_border, p_life_color, vector<string> {"Vertices", "Uvs"}, "Array"));
	}
}

void BillboardToolModel::draw() {
	glViewport(border[0], border[1], border[2], border[3]);
	shader_ptr->useShader();
	{
		const Camera& camera = *WindowManager::getWindowManager().currentCamera();
		const mat4 &projection_matrix = camera.getProjectionMatrix();
		const mat4 &view_matrix = camera.getViewMatrix();
		mat4 VP = projection_matrix * view_matrix;

		glUniform3f(shader_ptr->getUniform("camera_right_worldspace"), view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
		glUniform3f(shader_ptr->getUniform("camera_up_worldspace"), view_matrix[0][1], view_matrix[1][1], view_matrix[2][1]);
		glUniformMatrix4fv(shader_ptr->getUniform("VP"), 1, GL_FALSE, &VP[0][0]);
		glUniform4fv(shader_ptr->getUniform("life_color"), 1, &life_color[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gl_obj->getTexture("Billboard"));
		glUniform1i(shader_ptr->getUniform("texture_in"), 0);

		for (const shared_ptr<BaseModelSpirit>& base_ptr : gl_obj->getSpirits()) {
			glUniform3fv(shader_ptr->getUniform("billboard_pos"), 1, &(base_ptr->spirit().getPos()[0]));
			glUniform2f(shader_ptr->getUniform("billboard_size"), base_ptr->spirit().getSize()[0], base_ptr->spirit().getSize()[1]);

			board_border[2] = base_ptr->spirit().getLife();
			glUniform4fv(shader_ptr->getUniform("border"), 1, &board_border[0]);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			gl_obj->draw(draw_vec, draw_obj);

			glDisable(GL_BLEND);
		}
	}
	shader_ptr->unuseShader();
}

}