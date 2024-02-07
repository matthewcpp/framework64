#include "framework64/project.h"

#include "framework64/matrix.h"
#include "framework64/vec4.h"

int fw64_project_point(Vec3* pt, float* modelview, float* projection, IVec2* view_size, Vec3* result) {
    Vec4 pt4;
    vec4_set(&pt4, pt->x, pt->y, pt->z, 1.0f);

    matrix_transform_vec4(modelview, &pt4);
    matrix_transform_vec4(projection, &pt4);

    if (pt4.w == 0.0f) {
        return 0;
    }

    vec4_div(&pt4, pt4.w, &pt4);

    pt4.x = (pt4.x + 1.0f) / 2.0f;
    pt4.y = (pt4.y + 1.0f) / 2.0f;
    pt4.z = (pt4.z + 1.0f) / 2.0f;

	/* Map x,y to viewport */
	pt4.x = pt4.x * view_size->x;
	pt4.y = pt4.y * view_size->y;

	result->x = pt4.x;
	result->y = pt4.y;
    result->z = pt4.z;

    return 1;
}