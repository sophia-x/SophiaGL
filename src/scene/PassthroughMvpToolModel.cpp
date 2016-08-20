#include <camera/Camera>

#include <scene/Model>
#include <scene/WindowManager>

namespace gl
{

shared_ptr<PassthroughMvpToolModel> PassthroughMvpToolModel::initTool(const vec4& p_border, GLenum p_mode, const vec4& p_color, bool use_index,
        const string &vertex_path, const string &fragment_path, const vector<string>& uniforms) {
	// Create GLObj
	shared_ptr<GLObj> obj_ptr = GLObj::getGLObj();
	{
		vector<string> draw_vec;
		// Add GLBuffer
		obj_ptr->addBuffer("Vertices", GLDataObj::getGLBuffer((void*)0, 0, 3));
		draw_vec.push_back("Vertices");

		// Add GLDraw
		if (!use_index) {
			obj_ptr->addDrawObj("Array", GLArrayDraw::getDrawObj(obj_ptr->getBuffer("Vertices"), p_mode));

			return shared_ptr<PassthroughMvpToolModel>(new PassthroughMvpToolModel(obj_ptr, GLShader::getShader(vertex_path,
			        fragment_path, uniforms), p_border, p_mode, p_color, draw_vec, "Array"));
		} else {
			obj_ptr->addBuffer("Indices", GLElementObj::getGLBuffer(0, 0));
			draw_vec.push_back("Indices");
			obj_ptr->addDrawObj("Element", GLElementDraw::getDrawObj(obj_ptr->getBuffer("Indices"), p_mode));

			return shared_ptr<PassthroughMvpToolModel>(new PassthroughMvpToolModel(obj_ptr, GLShader::getShader(vertex_path,
			        fragment_path, uniforms), p_border, p_mode, p_color, draw_vec, "Element"));
		}
	}
}

void PassthroughMvpToolModel::draw() const {
	glViewport(border[0], border[1], border[2], border[3]);
	shader_ptr->useShader();
	{	
		setter->setup();

		const Camera& camera = *WindowManager::getWindowManager().currentCamera();
		const mat4 &projection_matrix = camera.getProjectionMatrix();
		const mat4 &view_matrix = camera.getViewMatrix();

		glUniform4fv(shader_ptr->getUniform("COLOR"), 1, &color[0]);

		const mat4 &model_matrix = spirit->getModelMatrix();
		mat4 MVP = projection_matrix * view_matrix * model_matrix;
		glUniformMatrix4fv(shader_ptr->getUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);

		gl_obj->draw(draw_vec, draw_obj);

	}
	shader_ptr->unuseShader();
}

}