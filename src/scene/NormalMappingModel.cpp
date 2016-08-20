#include <camera/Camera>

#include <scene/Model>
#include <scene/WindowManager>

#include <tools/obj_loader>

namespace gl
{

shared_ptr<NormalMappingModel> NormalMappingModel::initModel(const string& obj_path, const vector<pair<string, string>>& p_textures,
        const string &vertex_path, const string &fragment_path, const vector<string>& uniforms) {
	// Create GLObj
	shared_ptr<GLObj> obj_ptr = GLObj::getGLObj();
	{
		// Add GLBuffer
		vector<unsigned short> indices;
		vector<vec3> vertices;
		vector<vec2> uvs;
		vector<vec3> normals;
		ObjLoader::load_obj(obj_path, indices, vertices, uvs, normals);
		vector<vec3> tangents(vertices.size());
		vector<vec3> bitangents(vertices.size());
		ObjLoader::computeTangentBasis(indices, vertices, uvs, normals, tangents, bitangents);

		obj_ptr->addBuffer("Vertices", GLDataObj::getGLBuffer(&vertices[0], vertices.size() * 3, 3));
		obj_ptr->addBuffer("Uvs", GLDataObj::getGLBuffer(&uvs[0], uvs.size() * 2, 2));
		obj_ptr->addBuffer("Normals", GLDataObj::getGLBuffer(&normals[0], normals.size() * 3, 3));
		obj_ptr->addBuffer("Tangents", GLDataObj::getGLBuffer(&tangents[0], tangents.size() * 3, 3));
		obj_ptr->addBuffer("Bitangents", GLDataObj::getGLBuffer(&bitangents[0], bitangents.size() * 3, 3));
		obj_ptr->addBuffer("Indices", GLElementObj::getGLBuffer(&indices[0], indices.size()));
		// Add Texture
		for (const pair<string, string>& key_path : p_textures)
			obj_ptr->addTexture(key_path.first, key_path.second);
		// Add GLDraw
		obj_ptr->addDrawObj("Element", GLElementDraw::getDrawObj(obj_ptr->getBuffer("Indices"), GL_TRIANGLES));
	}

	return shared_ptr<NormalMappingModel>(new NormalMappingModel(obj_ptr, GLShader::getShader(vertex_path, fragment_path, uniforms),
	                                      vector<string> {"Vertices", "Uvs", "Normals", "Tangents", "Bitangents", "Indices"}, "Element"));
}

void NormalMappingModel::draw() const {
	setter->setup();

	const Camera& camera = *WindowManager::getWindowManager().currentCamera();
	const mat4 &projection_matrix = camera.getProjectionMatrix();
	const mat4 &view_matrix = camera.getViewMatrix();

	material->setUniforms(shader_ptr);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glUniform1i(shader_ptr->getUniform("diffuse_texture"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal_id);
	glUniform1i(shader_ptr->getUniform("normal_texture"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, specular_id);
	glUniform1i(shader_ptr->getUniform("specular_texture"), 2);

	const mat4 &model_matrix = spirit->getModelMatrix();
	mat4 MVP = projection_matrix * view_matrix * model_matrix;
	glUniformMatrix4fv(shader_ptr->getUniform("MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(shader_ptr->getUniform("M"), 1, GL_FALSE, &model_matrix[0][0]);
	glUniformMatrix4fv(shader_ptr->getUniform("V"), 1, GL_FALSE, &view_matrix[0][0]);
	mat3 ModelView3x3Matrix = mat3(view_matrix * model_matrix);
	glUniformMatrix3fv(shader_ptr->getUniform("MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);

	gl_obj->draw(draw_vec, draw_obj);
}

}