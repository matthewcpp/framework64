#include "framework64/collision.h"

#include "framework64/math.h"
#include "framework64/types.h"

static inline void swapf(float* a, float* b) {
    float temp = *a;
    *a = *b;
    *b = temp;
}

// Real Time Collision Detection 5.3.3
int fw64_collision_test_ray_box(const Vec3* origin, const Vec3* dir, const Box* box, Vec3* out_point, float* out_t) {
    const float* dir_el = (const float*)(dir);
    const float* origin_el = (const float*)origin;
    const float* box_min_el = (const float*) &box->min;
    const float* box_max_el = (const float*) &box->max;

    float tmin = 0.0f;          // set to -FLT_MAX to get first hit on line
    float tmax = FLT_MAX;       // set to max distance ray can travel (for segment)

    // For all three slabs
    for (int i = 0; i < 3; i++) {
        if (fabsf(dir_el[i]) < EPSILON) {
            // Ray is parallel to slab. No hit if origin not within slab
            if (origin_el[i] < box_min_el[i] || origin_el[i] > box_max_el[i]) {
                return 0;
            }
        } else {
            // Compute intersection t value of ray with near and far plane of slab
            float ood = 1.0f / dir_el[i];
            float t1 = (box_min_el[i] - origin_el[i]) * ood;
            float t2 = (box_max_el[i] - origin_el[i]) * ood;
            // Make t1 be intersection with near plane, t2 with far plane
            if (t1 > t2) {
                swapf(&t1, &t2);
            }
            // Compute the intersection of slab intersections intervals
            tmin = tmin > t1 ? tmin : t1; // tmin = Max(tmin, t1);
            tmax = tmax < t2 ? tmax : t2; // tmax = Min(tmax, t2);
            // Exit with no collision as soon as slab intersection becomes empty
            if (tmin > tmax) {
                return 0;
            }
        }
    }

    // Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
    vec3_add_and_scale(origin, dir, tmin, out_point);
    *out_t = tmin;
    return 1;
}

// Real Time Collision Detection 5.3.2
int fw64_collision_test_ray_sphere(const Vec3* origin, const Vec3* direction, const Vec3* center, float radius, Vec3* point, float* t) {
    Vec3 m;
    vec3_subtract(origin, center, &m);
    float b = vec3_dot(&m, direction);
    float c = vec3_dot(&m, &m) - (radius * radius);
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
    vec3_add_and_scale(origin, direction, *t, point);
    return 1;
}

// https://iquilezles.org/articles/intersectors/
int fw64_collision_test_ray_capsule(Vec3* origin, Vec3* direction, // ray
                                    Vec3* point_a, Vec3* point_b, float radius, // capsule
                                    Vec3* out_point, float* out_t) { //output
    Vec3  ba;
    vec3_subtract(point_b, point_a, &ba);
    Vec3  oa;
    vec3_subtract(origin, point_a, &oa);
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
        float t = (-b-fw64_sqrtf(h))/a;
        float y = baoa + t*bard;
        // body
        if( y>0.0 && y<baba ) {
            *out_t = t;
            vec3_add_and_scale(origin, direction, *out_t, out_point);  
            return 1;
        }
        // caps
        Vec3 oc;
        if (y <= 0.0) {
            vec3_copy(&oa, &oc);
        } else {
            vec3_subtract(origin, point_b, &oc);
        } 
        b = vec3_dot(direction,&oc);
        c = vec3_dot(&oc,&oc) - (radius*radius);
        h = b*b - c;
        if( h>0.0 ) {
            *out_t = -b - fw64_sqrtf(h);
            vec3_add_and_scale(origin, direction, *out_t, out_point);
            return 1;
        }
    }
    // no real roots, no intersection
    return 0;
}

// Real Time Collision Detection 5.2.5
int fw64_collision_test_box_sphere(const Box* box, const Vec3* center, float radius, Vec3* p) {
    box_closest_point(box, center, p);

    Vec3 v;
    vec3_subtract(p, center, &v);
    return vec3_dot(&v, &v) < radius * radius;
}

// Real Time Collision Detection 5.1.5
void fw64_closest_point_to_triangle(const Vec3* p, const Vec3* a, const Vec3* b, const Vec3* c, Vec3* out) {
    // Check if P in vertex region outside A
    Vec3 ab, ac, ap;
    vec3_subtract(b, a, &ab);
    vec3_subtract(c, a, &ac);
    vec3_subtract(p, a, &ap);

    float d1 = vec3_dot(&ab, &ap);
    float d2 = vec3_dot(&ac, &ap);
    if (d1 <= 0.0f && d2 <= 0.0f) {
        *out = *a; // barycentric coordinates (1,0,0)
        return;
    }

    // Check if P in vertex region outside B
    Vec3 bp;
    vec3_subtract(p, b, &bp);
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
        vec3_add_and_scale(a, &ab, v, out); // barycentric coordinates (1-v,v,0)
        return;
    }

    // Check if P in vertex region outside C
    Vec3 cp;
    vec3_subtract(p, c, &cp);
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
        vec3_add_and_scale(a, &ac, w, out); // barycentric coordinates (1-w,0,w)
        return;
    }

    // Check if P in edge region of BC, if so return projection of P onto BC
    float va = d3*d6 - d5*d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        Vec3 bc;
        vec3_subtract(c, b, &bc);
        vec3_add_and_scale(b, &bc, w, out); // barycentric coordinates (0,1-w,w)
        return;
    }

    // P inside face region. Compute Q through its barycentric coordinates (u,v,w)
    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;

    // return u*a + v*b + w*c, u = va * denom = 1.0f - v - w
    vec3_add_and_scale(a, &ab, v, out);
    vec3_add_and_scale(out, &ac, w, out);
}

void fw64_closest_point_on_line_segment(const Vec3* a, const Vec3* b, const Vec3* point, Vec3* out) {
    const float line_dist = vec3_distance_squared(a, b);
    if (line_dist <= EPSILON) {
        vec3_copy(a, out);
        return;
    }

    float t = ((point->x - a->x) * (b->x - a->x) + (point->y - a->y) * (b->y - a->y) + (point->z - a->z) * (b->z - a->z)) / line_dist;
    t = fw64_clamp(t, 0.0f, 1.0f);
    out->x = a->x + t * (b->x - a->x);
    out->y = a->y + t * (b->y - a->y); 
    out->z = a->z + t * (b->z - a->z);
}

// Real Time Collision Detection 5.2.7
int fw64_collision_test_sphere_triangle(const Vec3* center, float radius, const Vec3* a, const Vec3* b, const Vec3* c, Vec3* point) {
    // Find point P on triangle ABC closest to sphere center
    fw64_closest_point_to_triangle(center, a, b, c, point);

    // Sphere and triangle intersect if the (squared) distance from sphere
    // center to point p is less than the (squared) sphere radius
    //return vec3_distance_squared(point, center) <= radius * radius;

    float distance_squared = vec3_distance_squared(point, center);
    return distance_squared < radius * radius;
}

// Moller-Trumbore algorithm: wikipedia
int fw64_collision_test_ray_triangle(const Vec3* origin, const Vec3* direction, const Vec3* vertex0, const Vec3* vertex1, const Vec3* vertex2, Vec3* out_point, float* out_t) {
    Vec3 edge1, edge2, h, s, q;
    float a,f,u,v;

    vec3_subtract(vertex1, vertex0, &edge1);
    vec3_subtract(vertex2, vertex0, &edge2);

    vec3_cross(direction, &edge2, &h);
    a = vec3_dot(&edge1, &h);

    if (a > -EPSILON && a < EPSILON){
        return 0;    // This ray is parallel to this triangle.
    }

    f = 1.0f / a;
    vec3_subtract(origin, vertex0, &s);
    u = f * vec3_dot(&s, &h);
    if (u < 0.0 || u > 1.0) {
        return 0;
    }

    vec3_cross(&s, &edge1, &q);
    v = f * vec3_dot(direction, &q);
    if (v < 0.0 || u + v > 1.0) {
        return 0;
    }

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * vec3_dot(&edge2, &q);
    if (t > EPSILON) {// ray intersection
        vec3_add_and_scale(origin, direction, t, out_point);
        *out_t = t;
        return 1;
    }
    else {// This means that there is a line intersection but not a ray intersection.
        return 0;
    }
}

// Real Time Collision Detection 5.5.8
// note function modified: https://gamedev.stackexchange.com/questions/144854/intersectmovingaabbaabb-weird-behavior
int fw64_collision_test_moving_boxes(Box* a, Vec3* va, Box* b, Vec3* vb, float* t_first, float* t_last) {
    float* a_max_el = (float*)&a->max;
    float* a_min_el = (float*)&a->min;
    float* b_max_el = (float*)&b->max;
    float* b_min_el = (float*)&b->min;

    // Exit early if a and b initially overlapping
    if (box_intersection(a, b)) {
        *t_first = 0.0f;
        *t_last = 0.0f;
        return 1;
    }

    // Use relative velocity; effectively treating 'a' as stationary
    Vec3 v;
    vec3_subtract(vb, va, &v);
    float* v_el = (float*)&v;
    // Initialize times of first and last contact
    float tfirst = 0.0f;
    float tlast = 1.0f;

    // For each axis, determine times of first and last contact, if any
    for (int i = 0; i < 3; i++) {
        if (v_el[i] < 0.0f) {
            if (b_max_el[i] < a_min_el[i])
                return 0; // Nonintersecting and moving apart
            if (a_max_el[i] < b_min_el[i])
                tfirst = fw64_maxf((a_max_el[i] - b_min_el[i]) / v_el[i], tfirst);
            if (b_max_el[i] > a_min_el[i])
                tlast  = fw64_minf((a_min_el[i] - b_max_el[i]) / v_el[i], tlast);
        }
        else if (v_el[i] > 0.0f) {
            if (b_min_el[i] > a_max_el[i])
                return 0; // Nonintersecting and moving apart
            if (b_max_el[i] < a_min_el[i])
                tfirst = fw64_maxf((a_min_el[i] - b_max_el[i]) / v_el[i], tfirst);
            if (a_max_el[i] > b_min_el[i])
                tlast = fw64_minf((a_max_el[i] - b_min_el[i]) / v_el[i], tlast);
        }
        else {
            if (b_max_el[i] < a_min_el[i] || a_max_el[i] < b_min_el[i])
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
// TODO: Fix this s is used as undefined
int fw64_collision_test_moving_spheres(Vec3* ca, float ra, Vec3* va, Vec3* cb, float rb, Vec3* vb, float* t)
{
    Vec3 s = vec3_zero(), v;
    vec3_subtract(cb, ca, &v);      // vector between the center of the 2 spheres
    vec3_subtract(vb, va, &v);      // relative motion of sphere b with respect to stationary sphere a
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
    vec3_copy(&b->max, &e.max);
    vec3_copy(&b->min, &e.min);
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

void fw64_collision_get_normal_box_point(Vec3* point, Box* box, Vec3* out_normal) {   
    if(point->x > box->min.x && point->x < box->max.x) {
        if(point->z <= box->min.z) { // NORTH FACE
            out_normal->z = -1.0f;
        } else if (point->z >= box->max.z) { // SOUTH FACE
            out_normal->z = 1.0f;
        }
    } else if(point->z > box->min.z && point->z < box->max.z) {
        if(point->x <= box->min.x) { // WEST FACE
            out_normal->x = -1.0f;
        } else if (point->x >= box->max.x) { // EAST FACE
            out_normal->x = 1.0f;
        }
    }
}

// Finds the closest points between segment 1 (p1 to q1) and segment 2 (p2 to q2).
// Stores the closest point on seg 1 in c1, and the closest point on seg 2 in c2.
// Returns the squared distance between c1 and c2.
float fw64_closest_points_segment_segment(
    const Vec3* p1, const Vec3* q1, 
    const Vec3* p2, const Vec3* q2, 
    Vec3* c1, Vec3* c2) 
{
    Vec3 d1, d2, r;
    vec3_subtract(q1, p1, &d1); // Direction of segment 1
    vec3_subtract(q2, p2, &d2); // Direction of segment 2
    vec3_subtract(p1, p2, &r);

    float a = vec3_dot(&d1, &d1); // Squared length of segment 1
    float e = vec3_dot(&d2, &d2); // Squared length of segment 2
    float f = vec3_dot(&d2, &r);

    // Check if both segments degenerate into points
    if (a <= 0.00001f && e <= 0.00001f) {
        *c1 = *p1;
        *c2 = *p2;
        Vec3 diff;
        vec3_subtract(c1, c2, &diff);
        return vec3_dot(&diff, &diff);
    }

    float s, t; // Parametric values for the closest points on the segments

    if (a <= 0.00001f) {
        // First segment degenerates into a point
        s = 0.0f;
        t = f / e;
        t = fw64_clamp(t, 0.0f, 1.0f);
    } else {
        float c = vec3_dot(&d1, &r);
        if (e <= 0.00001f) {
            // Second segment degenerates into a point
            t = 0.0f;
            s = fw64_clamp(-c / a, 0.0f, 1.0f);
        } else {
            // General non-degenerate case
            float b = vec3_dot(&d1, &d2);
            float denom = a * e - b * b;

            // If segments are not parallel, compute closest point on L1 to L2 and clamp to S1
            if (denom != 0.0f) {
                s = fw64_clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            } else {
                s = 0.0f; // Arbitrary point, parallel segments
            }

            // Compute point on L2 closest to S1(s)
            t = (b * s + f) / e;

            // If t is outside [0,1], clamp and recompute s
            if (t < 0.0f) {
                t = 0.0f;
                s = fw64_clamp(-c / a, 0.0f, 1.0f);
            } else if (t > 1.0f) {
                t = 1.0f;
                s = fw64_clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    // Calculate the closest points
    Vec3 temp;
    vec3_scale(&d1, s, &temp);
    vec3_add(p1, &temp, c1);

    vec3_scale(&d2, t, &temp);
    vec3_add(p2, &temp, c2);

    // Return the squared distance
    Vec3 diff;
    vec3_subtract(c1, c2, &diff);
    return vec3_dot(&diff, &diff);
}


// Returns 1 if point p is inside triangle ABC, 0 otherwise.
// Assumes point p is already coplanar with the triangle.
static int fw64_point_in_triangle(const Vec3* p, const Vec3* a, const Vec3* b, const Vec3* c) {
    Vec3 v0, v1, v2;
    vec3_subtract(c, a, &v0);
    vec3_subtract(b, a, &v1);
    vec3_subtract(p, a, &v2);

    float d00 = vec3_dot(&v0, &v0);
    float d01 = vec3_dot(&v0, &v1);
    float d11 = vec3_dot(&v1, &v1);
    float d20 = vec3_dot(&v2, &v0);
    float d21 = vec3_dot(&v2, &v1);

    float denom = d00 * d11 - d01 * d01;
    
    // Check for degenerate triangle (division by zero)
    if (denom == 0.0f) {
        return 0; 
    }

    float invDenom = 1.0f / denom;
    float v = (d11 * d20 - d01 * d21) * invDenom;
    float w = (d00 * d21 - d01 * d20) * invDenom;

    // Point is inside the triangle if v and w are positive and v + w <= 1
    return (v >= 0.0f) && (w >= 0.0f) && (v + w <= 1.0f);
}

int fw64_collision_test_capsule_triangle(const fw64Capsule* capsule, const Vec3* tri_a, const Vec3* tri_b, const Vec3* tri_c, const Vec3* tri_n, Vec3* out_tri_point, Vec3* out_capsule_point) {
float radius_sq = capsule->radius * capsule->radius;
    float best_dist_sq = 9999999.0f; // Initialize to a large number
    Vec3 best_tri_pt = vec3_zero();
    Vec3 best_cap_pt = vec3_zero();

    // 2. Compute plane 'd' for the triangle
    // PERFORMANCE NOTE: You can speed this up by caching 'd' directly 
    // inside fw64CollisionTriangle during your mesh processing phase.
    float d = -vec3_dot(tri_n, tri_a);

    // --- Step 3: Check Capsule Spine vs Triangle Face (Piercing) ---
    Vec3 spine_dir;
    vec3_subtract(&capsule->b, &capsule->a, &spine_dir);

    // Distance from point to plane = dot(Normal, Point) + d
    float dist_a = vec3_dot(tri_n, &capsule->a) + d;
    float dist_b = vec3_dot(tri_n, &capsule->b) + d;

    // If dist_a and dist_b have different signs (or are zero), the segment crosses the plane
    if ((dist_a * dist_b) <= 0.0f) {
        float denom = dist_a - dist_b;
        
        // Protect against division by zero if the spine is perfectly parallel to the plane
        if (denom != 0.0f) { 
            float t = dist_a / denom;
            
            Vec3 intersection_pt;
            vec3_scale(&spine_dir, t, &intersection_pt);
            vec3_add(&capsule->a, &intersection_pt, &intersection_pt);

            // If the intersection point is inside the triangle boundaries, 
            // the spine pierces the triangle directly. Distance is exactly 0.
            if (fw64_point_in_triangle(&intersection_pt, tri_a, tri_b, tri_c)) {
                *out_tri_point = intersection_pt;
                *out_capsule_point = intersection_pt;
                return 1; 
            }
        } else {
            // Edge case: Spine lies exactly ON the plane. 
            if (fw64_point_in_triangle(&capsule->a, tri_a, tri_b, tri_c)) {
                *out_tri_point = capsule->a;
                *out_capsule_point = capsule->a;
                return 1;
            }
        }
    }

    // --- Step 4: Check Capsule Endpoints vs Triangle Surface ---
    Vec3 pt_on_tri;
    
    // Endpoint A
    fw64_closest_point_to_triangle(&capsule->a, tri_a, tri_b, tri_c, &pt_on_tri);
    float dist_sq_a = vec3_distance_squared(&capsule->a, &pt_on_tri);
    if (dist_sq_a < best_dist_sq) { 
        best_dist_sq = dist_sq_a; 
        best_tri_pt = pt_on_tri; 
        best_cap_pt = capsule->a; // The sphere center is endpoint A
    }

    // Endpoint B
    fw64_closest_point_to_triangle(&capsule->b, tri_a, tri_b, tri_c, &pt_on_tri);
    float dist_sq_b = vec3_distance_squared(&capsule->b, &pt_on_tri);
    if (dist_sq_b < best_dist_sq) { 
        best_dist_sq = dist_sq_b; 
        best_tri_pt = pt_on_tri; 
        best_cap_pt = capsule->b; // The sphere center is endpoint B
    }

    // --- Step 5: Check Capsule Spine vs Triangle Edges ---
    Vec3 c1, c2; // c1 on spine (capsule pt), c2 on triangle edge (tri pt)
    float dist_sq_edge;

    // Edge AB
    dist_sq_edge = fw64_closest_points_segment_segment(&capsule->a, &capsule->b, tri_a, tri_b, &c1, &c2);
    if (dist_sq_edge < best_dist_sq) { 
        best_dist_sq = dist_sq_edge; 
        best_tri_pt = c2; 
        best_cap_pt = c1; 
    }

    // Edge BC
    dist_sq_edge = fw64_closest_points_segment_segment(&capsule->a, &capsule->b, tri_b, tri_c, &c1, &c2);
    if (dist_sq_edge < best_dist_sq) { 
        best_dist_sq = dist_sq_edge; 
        best_tri_pt = c2; 
        best_cap_pt = c1; 
    }

    // Edge CA
    dist_sq_edge = fw64_closest_points_segment_segment(&capsule->a, &capsule->b, tri_c, tri_a, &c1, &c2);
    if (dist_sq_edge < best_dist_sq) { 
        best_dist_sq = dist_sq_edge; 
        best_tri_pt = c2; 
        best_cap_pt = c1; 
    }

    // 6. Final Evaluation
    if (best_dist_sq <= radius_sq) {
        *out_tri_point = best_tri_pt;
        *out_capsule_point = best_cap_pt;
        return 1;
    }

    return 0;
}
