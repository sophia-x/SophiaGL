#include <debug>

#include <demo>

int main() {

#ifdef DEBUG
	cout << "DEBUG" << endl;
#endif

	// standard_demo();
	// rotation_demo();
	// normal_mapping_demo();
	// particle_demo();
	// shadow_map_demo();
	// billboard_demo();
	// pick_by_bullet_demo();
	// pick_by_udf_demo();
	// pick_by_color_demo();
	wood_demo();

	return 0;
}