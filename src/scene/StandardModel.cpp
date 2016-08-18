#include <camera/Camera>

#include <scene/Model>
#include <scene/WindowManager>

#include <tools/obj_loader>

namespace gl
{

shared_ptr<StandardModel> StandardModel::getModel(const string& obj_path, const vector<pair<string, string>>& p_textures) {
	// Create GLObj
	shared_ptr<GLObj> obj_ptr = GLObj::getGLObj();
	{
		// Add GLBuffer
		vector<unsigned short> indices;
		vector<vec3> vertices;
		vector<vec2> uvs;
		vector<vec3> normals;
		ObjLoader::load_obj(obj_path, indices, vertices, uvs, normals);
		obj_ptr->addBuffer("Vertices", GLDataObj::getGLBuffer(&vertices[0], vertices.size() * 3, 3));
		obj_ptr->addBuffer("Uvs", GLDataObj::getGLBuffer(&uvs[0], uvs.size() * 2, 2));
		obj_ptr->addBuffer("Normals", GLDataObj::getGLBuffer(&normals[0], normals.size() * 3, 3));
		obj_ptr->addBuffer("Indices", GLElementObj::getGLBuffer(&indices[0], indices.size()));
		// Add Texture
		for (const pair<string, string>& key_path : p_textures)
			obj_ptr->addTexture(key_path.first, key_path.second);
		// Add GLDraw
		obj_ptr->addDrawObj("Element", GLElementDraw::getDrawObj(obj_ptr->getBuffer("Indices"), GL_TRIANGLES));
	}

	return shared_ptr<StandardModel>(new StandardModel(obj_ptr, vector<string> {"Vertices", "Uvs", "Normals", "Indices"}, "Element"));
}

void StandardModel::draw(const shared_ptr<GLShader>& shader_ptr) const {
	const Camera& camera = *WindowManager::getWindowManager().currentCamera();
	const mat4 &projection_matrix = camera.getProjectionMatrix();
	const mat4 &view_matrix = camera.getViewMatrix();

	for (const shared_ptr<BaseModelSpirit>& base_ptr : spirits) {
		StandardModelSpirit& model_spirit = (StandardModelSpirit&)(*base_ptr);
		Spirit& spirit = model_spirit.spirit();

		model_spirit.getMaterial().setUniforms(vector<GLuint> {
			shader_ptr->getUniform("material_ambient_color_ratio"),
			shader_ptr->getUniform("material_specular_color"),
			shader_ptr->getUniform("material_specular_ratio")
		});

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model_spirit.getTexture());
		glUniform1i(shader_ptr->getUniform("texture_in"), 0);

		const mat4 &model_matrix = spirit.getModelMatrix();
		mat4 MVP = projection_matrix * view_matrix * model_matrix;
		glUniformMatrix4fv(shader_ptr->getUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(shader_ptr->getUniform("M"), 1, GL_FALSE, &model_matrix[0][0]);
		glUniformMatrix4fv(shader_ptr->getUniform("V"), 1, GL_FALSE, &view_matrix[0][0]);

		gl_obj->draw(draw_vec, draw_obj);
	}
}

}