#include "framework64/collision.h"

#include "framework64/math.h"
#include "framework64/types.h"

static inline void swapf(float* a, float* b) {
    float temp = *a;
    *a = *b;
    *b = temp;
}

// Real Time Collision Detection 5.3.3
int fw64_collision_test_ray_box(Vec3* origin, Vec3* dir, Box* box, Vec3* out_point, float* out_t) {
    float tmin = 0.0f;          // set to -FLT_MAX to get first hit on line
    float tmax = FLT_MAX;       // set to max distance ray can travel (for segment)

    // For all three slabs
    for (int i = 0; i < 3; i++) {
        if (fabsf(dir->el[i]) < EPSILON) {
            // Ray is parallel to slab. No hit if origin not within slab
            if (origin->el[i] < box->min.el[i] || origin->el[i] > box->max.el[i])
                return 0;
        } else {
            // Compute intersection t value of ray with near and far plane of slab
            float ood = 1.0f / dir->el[i];
            float t1 = (box->min.el[i] - origin->el[i]) * ood;
            float t2 = (box->max.el[i] - origin->el[i]) * ood;
            // Make t1 be intersection with near plane, t2 with far plane
            if (t1 > t2)
                swapf(&t1, &t2);
            // Compute the intersection of slab intersections intervals
            tmin = tmin > t1 ? tmin : t1; // tmin = Max(tmin, t1);
            tmax = tmax < t2 ? tmax : t2; // tmax = Min(tmax, t2);
            // Exit with no collision as soon as slab intersection becomes empty
            if (tmin > tmax)
                return 0;
        }
    }

    // Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
    vec3_add_and_scale(out_point, origin, dir, tmin);
    *out_t = tmin;
    return 1;
}

// Real Time Collision Detection 5.2.5
int fw64_collision_test_box_sphere(Box* box, Vec3* center, float radius, Vec3* p) {
    box_closest_point(box, center, p);

    Vec3 v;
    vec3_subtract(&v, p, center);
    return vec3_dot(&v, &v) < radius * radius;
}

// Real Time Collision Detection 5.1.5
void fw64_closest_point_to_triangle(Vec3* p, Vec3* a, Vec3* b, Vec3* c, Vec3* out) {
    // Check if P in vertex region outside A
    Vec3 ab, ac, ap;
    vec3_subtract(&ab, b, a);
    vec3_subtract(&ac, c, a);
    vec3_subtract(&ap, p, a);

    float d1 = vec3_dot(&ab, &ap);
    float d2 = vec3_dot(&ac, &ap);
    if (d1 <= 0.0f && d2 <= 0.0f) {
        *out = *a; // barycentric coordinates (1,0,0)
        return;
    }

    // Check if P in vertex region outside B
    Vec3 bp;
    vec3_subtract(&bp, p, b);
    float d3 = vec3_dot(&ab, &bp);
    float d4 = vec3_dot(&ac, &bp);
    if (d3 >= 0.0f && d4 <= d3) {
        *out = *b; // barycentric coordinates (0,1,0)
        return;
    }

    // Check if P in edge region of AB, if so return projection of P onto AB
    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        float v = d1 / (d1 - d3);
        vec3_add_and_scale(out, a, &ab, v); // barycentric coordinates (1-v,v,0)
        return;
    }

    // Check if P in vertex region outside C
    Vec3 cp;
    vec3_subtract(&cp, p, c);
    float d5 = vec3_dot(&ab, &cp);
    float d6 = vec3_dot(&ac, &cp);
    if (d6 >= 0.0f && d5 <= d6) {
        *out = *c; // barycentric coordinates (0,0,1)
        return;
    }

    // Check if P in edge region of AC, if so return projection of P onto AC
    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        float w = d2 / (d2 - d6);
        vec3_add_and_scale(out, a, &ac, w); // barycentric coordinates (1-w,0,w)
        return;
    }

    // Check if P in edge region of BC, if so return projection of P onto BC
    float va = d3*d6 - d5*d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        Vec3 bc;
        vec3_subtract(&bc, c, b);
        vec3_add_and_scale(out, b, &bc, w); // barycentric coordinates (0,1-w,w)
        return;
    }

    // P inside face region. Compute Q through its barycentric coordinates (u,v,w)
    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;

    // return u*a + v*b + w*c, u = va * denom = 1.0f - v - w
    vec3_add_and_scale(out, a, &ab, v);
    vec3_add_and_scale(out, out, &ac, w);
}

// Real Time Collision Detection 5.2.7
int fw64_collision_test_sphere_triangle(Vec3* center, float radius, Vec3* a, Vec3* b, Vec3* c, Vec3* point) {
    // Find point P on triangle ABC closest to sphere center
    fw64_closest_point_to_triangle(center, a, b, c, point);

    // Sphere and triangle intersect if the (squared) distance from sphere
    // center to point p is less than the (squared) sphere radius
    Vec3 v;
    vec3_subtract(&v, point, center);
    return vec3_dot(&v, &v) <= radius * radius;
}

// Moller-Trumbore algorithm: wikipedia
int fw64_collision_test_ray_triangle(Vec3* origin, Vec3* direction, Vec3* vertex0, Vec3* vertex1, Vec3* vertex2, Vec3* out_point, float* out_t) {
    Vec3 edge1, edge2, h, s, q;
    float a,f,u,v;

    vec3_subtract(&edge1, vertex1, vertex0);
    vec3_subtract(&edge2, vertex2, vertex0);

    vec3_cross(&h, direction, &edge2);
    a = vec3_dot(&edge1, &h);

    if (a > -EPSILON && a < EPSILON)
        return 0;    // This ray is parallel to this triangle.

    f = 1.0f / a;
    vec3_subtract(&s, origin, vertex0);
    u = f * vec3_dot(&s, &h);
    if (u < 0.0 || u > 1.0)
        return 0;

    vec3_cross(&q, &s, &edge1);
    v = f * vec3_dot(direction, &q);
    if (v < 0.0 || u + v > 1.0)
        return 0;


    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * vec3_dot(&edge2, &q);
    if (t > EPSILON) // ray intersection
    {
        vec3_add_and_scale(out_point, origin, direction, t);
        *out_t = t;
        return 1;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return 0;
}

// Real Time Collision Detection 5.5.8
// note function modified: https://gamedev.stackexchange.com/questions/144854/intersectmovingaabbaabb-weird-behavior
int fw64_collision_test_moving_boxes(Box* a, Vec3* va, Box* b, Vec3* vb, float* t_first, float* t_last) {
    // Exit early if a and b initially overlapping
    if (box_intersection(a, b)) {
        *t_first = 0.0f;
        *t_last = 0.0f;
        return 1;
    }

    // Use relative velocity; effectively treating 'a' as stationary
    Vec3 v;
    vec3_subtract(&v, vb, va);

    // Initialize times of first and last contact
    float tfirst = 0.0f;
    float tlast = 1.0f;

    // For each axis, determine times of first and last contact, if any
    for (int i = 0; i < 3; i++) {
        if (v.el[i] < 0.0f) {
            if (b->max.el[i] < a->min.el[i])
                return 0; // Nonintersecting and moving apart
            if (a->max.el[i] < b->min.el[i])
                tfirst = fw64_maxf((a->max.el[i] - b->min.el[i]) / v.el[i], tfirst);
            if (b->max.el[i] > a->min.el[i])
                tlast  = fw64_minf((a->min.el[i] - b->max.el[i]) / v.el[i], tlast);
        }
        else if (v.el[i] > 0.0f) {
            if (b->min.el[i] > a->max.el[i])
                return 0; // Nonintersecting and moving apart
            if (b->max.el[i] < a->min.el[i])
                tfirst = fw64_maxf((a->min.el[i] - b->max.el[i]) / v.el[i], tfirst);
            if (a->max.el[i] > b->min.el[i])
                tlast = fw64_minf((a->max.el[i] - b->min.el[i]) / v.el[i], tlast);
        }
        else {
            if (b->max.el[i] < a->min.el[i] || a->max.el[i] < b->min.el[i])
                return 0;
        }

        // No overlap possible if time of first contact occurs after time of last contact
        if (tfirst > tlast) return 0;
    }

    *t_first = tfirst;
    *t_last = tlast;

    return 1;
}