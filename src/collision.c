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

// Real Time Collision Detection 5.3.2
int fw64_collision_test_ray_sphere(Vec3* origin, Vec3* direction, Vec3* center, float radius, Vec3* point, float* t) {
    Vec3 m;
    vec3_subtract(&m, origin, center);
    float b = vec3_dot(&m, direction);
    float c = vec3_dat(&m, &m) - (radius * radius);
    // exit early. ray origin outside sphere, and direction points away from sphere
    if (c > 0.0f && b > 0.0f) return 0;
    
    float discr = (b*b) - c;
    // discriminant <0 means ray misses
    if (discr < 0.0f) return 0;
    
    // ray intersect sphere, find smallest t value of intersection
    *t = -b - fw64_sqrtf(discr);
    // t < 0, ray started inside sphere. set t = 0
    if (*t < 0.0f) *t = 0.0f;
    // output point of first collision
    vec3_add_and_scale(point, origin, direction, *t);
    return 1;
}

// https://iquilezles.org/articles/intersectors/
int fw64_collision_test_ray_capsule(Vec3* origin, Vec3* direction, // ray
                                    Vec3* point_a, Vec3* point_b, float radius, // capsule
                                    Vec3* out_point, float* out_t) { //output
    Vec3  ba;
    vec3_subtract(&ba, point_b, point_a);
    Vec3  oa;
    vec3_subtract(&oa, origin, point_a);
    float baba = vec3_dot(&ba, &ba);
    float bard = vec3_dot(&ba, direction);
    float baoa = vec3_dot(&ba,&oa);
    float rdoa = vec3_dot(direction, &oa);
    float oaoa = vec3_dot(&oa, &oa);
    float a = baba      - bard*bard;
    float b = baba*rdoa - baoa*bard;
    float c = baba*oaoa - baoa*baoa - radius*radius*baba;
    float h = b*b - a*c;
    
    if( h >= 0.0 )
    {
        float t = (-b-sqrt(h))/a;
        float y = baoa + t*bard;
        // body
        if( y>0.0 && y<baba ) {
            *out_t = t;
            vec3_add_and_scale(out_point, origin, direction, *out_t);  
            return 1;
        }
        // caps
        Vec3 oc;
        if (y <= 0.0) {
            vec3_copy(&oc, &oa);
        } else {
            vec3_subtract(&oc, origin, point_b);
        } 
        b = vec3_dot(direction,&oc);
        c = vec3_dot(&oc,&oc) - (radius*radius);
        h = b*b - c;
        if( h>0.0 ) {
            *out_t = -b - fw64_sqrtf(h);
            vec3_add_and_scale(out_point, origin, direction, *out_t);
            return 1;
        }
    }
    // no real roots, no intersection
    return 0;
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

// Real Time Collision Detection 5.5.5, Intersecting Moving Sphere Against Sphere
int fw64_collision_test_moving_spheres(Vec3* ca, float ra, Vec3* va, Vec3* cb, float rb, Vec3* vb, float* t)
{
    Vec3 s, v;
    vec3_subtract(&s, cb, ca);      // vector between the center of the 2 spheres
    vec3_subtract(&v, vb, va);      // relative motion of sphere b with respect to stationary sphere a
    float r = rb + ra;              // the sum of all spheres (radii)
    float c = vec3_dot(&s, &s) - (r * r);
    if (c < 0.0f) { // if spheres overlap already, without any motion
        *t = 0.0f; 
        return 1;
    }
    float a = vec3_dot(&v, &v);
    if (a < EPSILON) return 0; // not moving relative each other
    float b = vec3_dot(&v, &s);
    if (b >= 0.0f) return 0;   // not moving towards each other
    float d = (b * b) - (a * c);
    if (d < 0.0f) return 0;    // quadratic function has no real roots, therefore no intersection

    *t = (-b - fw64_sqrtf(d)) / a;
    return 1;
}

// Support function for moving sphere box test.
// get the AABB vertex with index n
static void get_vertex(Box* b, int n, Vec3* out_vertex)
{
    vec3_set(out_vertex,
             ((n & 1) ? b->max.x : b->min.x),
             ((n & 2) ? b->max.y : b->min.y),
             ((n & 4) ? b->max.z : b->min.z));
}

// Real Time Collision Detection 5.5.7
int fw64_collision_test_moving_sphere_box(Vec3* center, float radius, Vec3* direction, Box* b, Vec3* out_point, float* out_t) {
    // find an expanded box, start with a duplicate of b, expand by radius in all directions
    // this is an expanded AABB with the same lengths along the primary axes as the minkowski sum
    // but it is *not* the minkowski sum (it doesn't have quarter-sphere edges) and is just a first-pass test
    Box e;
    vec3_copy(&e.max, &b->max);
    vec3_copy(&e.min, &b->min);
    e.min.x -= radius; e.min.y -= radius; e.min.z -= radius;
    e.max.x += radius; e.max.y += radius; e.max.z += radius;

    // Intersect ray against expanded AABB e.
    //if ray misses e, no intersection possible. Exit early
    //else continue and get intersection point p and time t as result
    *out_t = FLT_MAX;
    if(!fw64_collision_test_ray_box(center, direction, &e, out_point, out_t) || *out_t > 1.0) {
        return 0;
    }

    // Compute which min and max faces of b the intersection point p lies
    // outside of. Note, u and v cannot have the same bits set and
    // they must have at least one bit set amongst them
    int u = 0, v = 0;
    if (out_point->x < b->min.x) u |= 1;
    if (out_point->x > b->max.x) v |= 1;
    if (out_point->y < b->min.y) u |= 2;
    if (out_point->y > b->max.y) v |= 2;
    if (out_point->z < b->min.z) u |= 4;
    if (out_point->z > b->max.z) v |= 4;

    // ‘Or’ all set bits together into a bit mask
    int m = u | v;

    // If only one bit set in m, then p is in a face region
    if ((m & (m - 1)) == 0) {
        // Do nothing. Time t from intersection with
        // expanded box is correct intersection time
        return 1;
    }

    float tmin = FLT_MAX;
    Vec3 vert_a, vert_b;

    // all 3 bits set (m == 7), p is in a vertex region
    if(m==7) {
        
        
        get_vertex(b, v, &vert_a); get_vertex(b, v ^ 1, &vert_b);
        if(fw64_collision_test_ray_capsule(center, direction,
                                           &vert_a, &vert_b, radius,
                                           out_point, out_t)) {
            tmin = fw64_minf(*out_t, tmin);
        }
        get_vertex(b, v ^ 2, &vert_b);
        if(fw64_collision_test_ray_capsule(center, direction,
                                           &vert_a, &vert_b, radius,
                                           out_point, out_t)) {
            tmin = fw64_minf(*out_t, tmin);
        }
        get_vertex(b, v ^ 4, &vert_b);
        if(fw64_collision_test_ray_capsule(center, direction,
                                           &vert_a, &vert_b, radius,
                                           out_point, out_t)) {
            tmin = fw64_minf(*out_t, tmin);
        }
        if(tmin == FLT_MAX) return 0; // no intersection found

        *out_t = tmin; // intersection found at t==tmin
        return 1;
    }

    // p is in an edge region. Intersect against the capsule at the edge
    get_vertex(b, u ^ 7, &vert_a);
    get_vertex(b, v, &vert_b);
    return fw64_collision_test_ray_capsule(center, direction,
                                        &vert_a, &vert_b, radius,
                                        out_point, out_t);
}