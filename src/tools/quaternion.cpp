#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#include <tools/quaternion>

#include <debug>

namespace Quat
{

const vec3 X(1, 0, 0);
const vec3 Y(0, 1, 0);
const vec3 Z(0, 0, 1);

// Returns a quaternion q: q*start = dest
quat rotationBetweenVectors(const vec3& start, const vec3& dest) {
	vec3 n_start = normalize(start);
	vec3 n_dest = normalize(dest);

	return rotation(n_start, n_dest);
}

quat lookAt(const vec3& direction, const vec3& up) {
	if (length2(direction) < 0.0001f )
		return quat();

	vec3 right = cross(direction, up);
	vec3 r_up = cross(right, direction);
	quat rot1 = rotationBetweenVectors(Z, direction);
	vec3 new_up = rot1 * Y;
	quat rot2 = rotationBetweenVectors(new_up, r_up);

	return rot2 * rot1;
}

quat rotateTowards(const quat& q1, const quat& q2, float max_angle) {
	if ( max_angle < 0.001f ) {
		return q1;
	}

	float cos_theta = dot(q1, q2);
	if (cos_theta > 0.9999f) {
		return q2;
	}

	quat r_q1 = q1;
	if (cos_theta < 0) {
		r_q1 = r_q1 * -1.0f;
		cos_theta *= -1.0f;
	}

	float angle = acos(cos_theta);
	if (angle < max_angle) {
		return q2;
	}

	float t = max_angle / angle;
	angle = max_angle;
	quat res = (sin((1.0f - t) * angle) * r_q1 + sin(t * angle) * q2) / sin(angle);
	res = normalize(res);

	return res;
}

}