// #include <camera/Camera>
// #include <camera/RotateCamera>

// #include <gl_objs/GLBuffer>
// #include <gl_objs/GLDraw>
// #include <gl_objs/GLFrame>
// #include <gl_objs/GLHeader>
// #include <gl_objs/GLObj>
// #include <gl_objs/GLShader>

// #include <scene/Material>
// #include <scene/Model>
// #include <scene/Scene>
// #include <scene/Spirit>
// #include <scene/Material>
// #include <scene/Light>
// #include <scene/WindowManager>
// #include <scene/ModelSpirit>

// #include <tools/callback_helper>
// #include <tools/obj_loader>
// #include <tools/quaternion>
// #include <tools/shader_loader>
// #include <tools/texture_loader>
// #include <tools/Timer>

#include <debug>

#include <demo>

int main() {

#ifdef DEBUG
	cout << "DEBUG" << endl;
#endif

	// standard_demo();
	// rotation_demo();
	normal_mapping_demo();

	return 0;
}