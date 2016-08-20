#include <camera/Camera>

#include <scene/Model>
#include <scene/WindowManager>

namespace gl
{

shared_ptr<VertexTextureToolModel> VertexTextureToolModel::initTool(const vec4& p_border, GLenum p_mode, GLuint texture_id,
        const string &vertex_path, const string &fragment_path, const vector<string>& uniforms) {
	// Create GLObj
	shared_ptr<GLObj> obj_ptr = GLObj::getGLObj();
	{
		// Add GLBuffer
		obj_ptr->addBuffer("Vertices", GLDataObj::getGLBuffer((void*)0, 0, 3));
		obj_ptr->addBuffer("Uvs", GLDataObj::getGLBuffer((void*)0, 0, 2));
		obj_ptr->addTexture("Texture", texture_id);
		obj_ptr->addDrawObj("Array", GLArrayDraw::getDrawObj(obj_ptr->getBuffer("Vertices"), p_mode));

		return shared_ptr<VertexTextureToolModel>(new VertexTextureToolModel(obj_ptr, GLShader::getShader(vertex_path,
		        fragment_path, uniforms), p_border, p_mode, vector<string> {"Vertices", "Uvs"}, "Array"));
	}
}

void VertexTextureToolModel::draw() const {
	glViewport(border[0], border[1], border[2], border[3]);
	shader_ptr->useShader();
	{	
		setter->setup();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gl_obj->getTexture("Texture"));
		glUniform1i(shader_ptr->getUniform("texture_in"), 0);

		const mat4 &model_matrix = spirit->getModelMatrix();
		glUniformMatrix4fv(shader_ptr->getUniform("M"), 1, GL_FALSE, &model_matrix[0][0]);

		gl_obj->draw(draw_vec, draw_obj);

	}
	shader_ptr->unuseShader();
}

}