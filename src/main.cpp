#include <gl_objs/GLBuffer>
#include <gl_objs/GLObj>
#include <gl_objs/GLShader>
#include <gl_objs/GLFrame>

int main() {

#ifdef DEBUG
	cout << "DEBUG" << endl;
#endif

	gl::GLDataObj::getGLBuffer(1);
	gl::GLStreamObj::getGLBuffer(1);
	gl::GLElementObj::getGLBuffer();

	gl::GLObj::getGLObj();

	gl::GLDepthFrame::getFrame(vec2());

	return 0;
}