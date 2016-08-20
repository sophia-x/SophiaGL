#include <camera/Camera>

#include <scene/Model>
#include <scene/WindowManager>

namespace gl
{

shared_ptr<TextToolModel> TextToolModel::initTool(const vec4& p_border, const vec2& p_size, const string& texture_path,
        const string &vertex_path, const string &fragment_path, const vector<string>& uniforms) {
	// Create GLObj
	shared_ptr<GLObj> obj_ptr = GLObj::getGLObj();
	{
		// Add GLBuffer
		obj_ptr->addBuffer("Vertices", GLDataObj::getGLBuffer((void*)0, 0, 2));
		obj_ptr->addBuffer("Uvs", GLDataObj::getGLBuffer((void*)0, 0, 2));
		// Add Texture
		obj_ptr->addTexture("TEXT", texture_path);
		// Add GLDraw
		obj_ptr->addDrawObj("Array", GLArrayDraw::getDrawObj(obj_ptr->getBuffer("Vertices"), GL_TRIANGLES));
	}

	return shared_ptr<TextToolModel>(new TextToolModel(obj_ptr, GLShader::getShader(vertex_path, fragment_path, uniforms), p_border, p_size,
	                                 vector<string> {"Vertices", "Uvs"}, "Array"));
}

void TextToolModel::print_text(const string& text, int x, int y, int size) {
	vector<vec2> vertices;
	vector<vec2> uvs;

	for (size_t i = 0 ; i < text.size() ; i++ ) {
		vec2 vertex_up_left    = vec2( x + i * size     , y + size );
		vec2 vertex_up_right   = vec2( x + i * size + size, y + size );
		vec2 vertex_down_right = vec2( x + i * size + size, y      );
		vec2 vertex_down_left  = vec2( x + i * size     , y      );

		vertices.push_back(vertex_up_left   );
		vertices.push_back(vertex_down_left );
		vertices.push_back(vertex_up_right  );

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		char character = text[i];
		float uv_x = (character % 16) / 16.0f;
		float uv_y = (character / 16) / 16.0f;

		vec2 uv_up_left    = vec2( uv_x           , uv_y );
		vec2 uv_up_right   = vec2( uv_x + 1.0f / 16.0f, uv_y );
		vec2 uv_down_right = vec2( uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f) );
		vec2 uv_down_left  = vec2( uv_x           , (uv_y + 1.0f / 16.0f) );

		uvs.push_back(uv_up_left   );
		uvs.push_back(uv_down_left );
		uvs.push_back(uv_up_right  );

		uvs.push_back(uv_down_right);
		uvs.push_back(uv_up_right);
		uvs.push_back(uv_down_left);
	}

	gl_obj->appendData("Vertices", &vertices[0], vertices.size() * 2);
	gl_obj->appendData("Uvs", &uvs[0], uvs.size() * 2);

	change = true;
}

void TextToolModel::draw() const {
	glViewport(border[0], border[1], border[2], border[3]);
	shader_ptr->useShader();
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		{
			setter->setup();
			glUniform1i(shader_ptr->getUniform("half_width"), size[0] / 2);
			glUniform1i(shader_ptr->getUniform("half_height"), size[1] / 2);

			// Bind texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, gl_obj->getTexture("TEXT"));
			glUniform1i(shader_ptr->getUniform("texture_in"), 0);

			gl_obj->draw(draw_vec, draw_obj);
		}
		glDisable(GL_BLEND);
	}
	shader_ptr->unuseShader();
}

}