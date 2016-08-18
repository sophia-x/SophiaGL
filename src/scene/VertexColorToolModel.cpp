#include <camera/Camera>

#include <scene/Model>
#include <scene/WindowManager>

namespace gl
{

shared_ptr<VertexColorToolModel> VertexColorToolModel::getTool(const vec4& p_border, GLenum p_mode,
        const string &vertex_path, const string &fragment_path, const vector<string>& uniforms) {
	// Create GLObj
	shared_ptr<GLObj> obj_ptr = GLObj::getGLObj();
	{
		// Add GLBuffer
		obj_ptr->addBuffer("Vertices", GLDataObj::getGLBuffer((void*)0, 0, 3));
		obj_ptr->addBuffer("Color", GLDataObj::getGLBuffer((void*)0, 0, 3));

		obj_ptr->addDrawObj("Array", GLArrayDraw::getDrawObj(obj_ptr->getBuffer("Vertices"), p_mode));

		return shared_ptr<VertexColorToolModel>(new VertexColorToolModel(obj_ptr, GLShader::getShader(vertex_path,
		                                        fragment_path, uniforms), p_border, p_mode, vector<string> {"Vertices", "Color"}, "Array"));
	}
}

void VertexColorToolModel::draw() {
	glViewport(border[0], border[1], border[2], border[3]);
	shader_ptr->useShader();
	{
		const Camera& camera = *WindowManager::getWindowManager().currentCamera();
		const mat4 &projection_matrix = camera.getProjectionMatrix();
		const mat4 &view_matrix = camera.getViewMatrix();

		for (const shared_ptr<BaseModelSpirit>& base_ptr : spirits) {
			const mat4 &model_matrix = base_ptr->spirit().getModelMatrix();
			mat4 MVP = projection_matrix * view_matrix * model_matrix;
			glUniformMatrix4fv(shader_ptr->getUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);

			gl_obj->draw(draw_vec, draw_obj);
		}
	}
	shader_ptr->unuseShader();
}

}