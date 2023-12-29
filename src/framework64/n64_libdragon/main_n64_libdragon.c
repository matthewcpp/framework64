
#include "framework64/n64_libdragon/engine.h"
#include "game.h"

Game game;
fw64LibdragonEngine engine;

#ifndef CAMERA_H
#define CAMERA_H

#include <libdragon.h>
#include <GL/gl.h>
#include <GL/glu.h>

typedef struct {
    float distance;
    float rotation;
} camera_t;

void camera_transform(const camera_t *camera)
{
    // Set the camera transform
    glLoadIdentity();
    gluLookAt(
        0, -camera->distance, -camera->distance,
        0, 0, 0,
        0, 1, 0);
    glRotatef(camera->rotation, 0, 1, 0);
}

#endif

#ifndef VERTEX
#define VERTEX

#include <stdint.h>

typedef struct {
    float position[3];
    float texcoord[2];
    float normal[3];
    uint32_t color;
} vertex_t;

#endif

#ifndef CUBE_H
#define CUBE_H

#include <libdragon.h>
#include <GL/gl.h>

static const float cube_size = 3.0f;

static const vertex_t cube_vertices[] = {
    // +X
    { .position = { cube_size, -cube_size, -cube_size}, .texcoord = {0.f, 0.f}, .normal = { 1.f,  0.f,  0.f}, .color = 0xFF0000FF },
    { .position = { cube_size,  cube_size, -cube_size}, .texcoord = {1.f, 0.f}, .normal = { 1.f,  0.f,  0.f}, .color = 0xFF0000FF },
    { .position = { cube_size,  cube_size,  cube_size}, .texcoord = {1.f, 1.f}, .normal = { 1.f,  0.f,  0.f}, .color = 0xFF0000FF },
    { .position = { cube_size, -cube_size,  cube_size}, .texcoord = {0.f, 1.f}, .normal = { 1.f,  0.f,  0.f}, .color = 0xFF0000FF },

    // -X
    { .position = {-cube_size, -cube_size, -cube_size}, .texcoord = {0.f, 0.f}, .normal = {-1.f,  0.f,  0.f}, .color = 0x00FFFFFF },
    { .position = {-cube_size, -cube_size,  cube_size}, .texcoord = {0.f, 1.f}, .normal = {-1.f,  0.f,  0.f}, .color = 0x00FFFFFF },
    { .position = {-cube_size,  cube_size,  cube_size}, .texcoord = {1.f, 1.f}, .normal = {-1.f,  0.f,  0.f}, .color = 0x00FFFFFF },
    { .position = {-cube_size,  cube_size, -cube_size}, .texcoord = {1.f, 0.f}, .normal = {-1.f,  0.f,  0.f}, .color = 0x00FFFFFF },

    // +Y
    { .position = {-cube_size,  cube_size, -cube_size}, .texcoord = {0.f, 0.f}, .normal = { 0.f,  1.f,  0.f}, .color = 0x00FF00FF },
    { .position = {-cube_size,  cube_size,  cube_size}, .texcoord = {0.f, 1.f}, .normal = { 0.f,  1.f,  0.f}, .color = 0x00FF00FF },
    { .position = { cube_size,  cube_size,  cube_size}, .texcoord = {1.f, 1.f}, .normal = { 0.f,  1.f,  0.f}, .color = 0x00FF00FF },
    { .position = { cube_size,  cube_size, -cube_size}, .texcoord = {1.f, 0.f}, .normal = { 0.f,  1.f,  0.f}, .color = 0x00FF00FF },

    // -Y
    { .position = {-cube_size, -cube_size, -cube_size}, .texcoord = {0.f, 0.f}, .normal = { 0.f, -1.f,  0.f}, .color = 0xFF00FFFF },
    { .position = { cube_size, -cube_size, -cube_size}, .texcoord = {1.f, 0.f}, .normal = { 0.f, -1.f,  0.f}, .color = 0xFF00FFFF },
    { .position = { cube_size, -cube_size,  cube_size}, .texcoord = {1.f, 1.f}, .normal = { 0.f, -1.f,  0.f}, .color = 0xFF00FFFF },
    { .position = {-cube_size, -cube_size,  cube_size}, .texcoord = {0.f, 1.f}, .normal = { 0.f, -1.f,  0.f}, .color = 0xFF00FFFF },

    // +Z
    { .position = {-cube_size, -cube_size,  cube_size}, .texcoord = {0.f, 0.f}, .normal = { 0.f,  0.f,  1.f}, .color = 0x0000FFFF },
    { .position = { cube_size, -cube_size,  cube_size}, .texcoord = {1.f, 0.f}, .normal = { 0.f,  0.f,  1.f}, .color = 0x0000FFFF },
    { .position = { cube_size,  cube_size,  cube_size}, .texcoord = {1.f, 1.f}, .normal = { 0.f,  0.f,  1.f}, .color = 0x0000FFFF },
    { .position = {-cube_size,  cube_size,  cube_size}, .texcoord = {0.f, 1.f}, .normal = { 0.f,  0.f,  1.f}, .color = 0x0000FFFF },

    // -Z
    { .position = {-cube_size, -cube_size, -cube_size}, .texcoord = {0.f, 0.f}, .normal = { 0.f,  0.f, -1.f}, .color = 0xFFFF00FF },
    { .position = {-cube_size,  cube_size, -cube_size}, .texcoord = {0.f, 1.f}, .normal = { 0.f,  0.f, -1.f}, .color = 0xFFFF00FF },
    { .position = { cube_size,  cube_size, -cube_size}, .texcoord = {1.f, 1.f}, .normal = { 0.f,  0.f, -1.f}, .color = 0xFFFF00FF },
    { .position = { cube_size, -cube_size, -cube_size}, .texcoord = {1.f, 0.f}, .normal = { 0.f,  0.f, -1.f}, .color = 0xFFFF00FF },
};

static const uint16_t cube_indices[] = {
     0,  1,  2,  0,  2,  3,
     4,  5,  6,  4,  6,  7,
     8,  9, 10,  8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23,
};

void setup_cube()
{
}

void draw_cube()
{
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(vertex_t), (void*)(0*sizeof(float) + (void*)cube_vertices));
    glTexCoordPointer(2, GL_FLOAT, sizeof(vertex_t), (void*)(3*sizeof(float) + (void*)cube_vertices));
    glNormalPointer(GL_FLOAT, sizeof(vertex_t), (void*)(5*sizeof(float) + (void*)cube_vertices));
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(vertex_t), (void*)(8*sizeof(float) + (void*)cube_vertices));

    glDrawElements(GL_TRIANGLES, sizeof(cube_indices) / sizeof(uint16_t), GL_UNSIGNED_SHORT, cube_indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void render_cube()
{
    rdpq_debug_log_msg("Cube");
    glPushMatrix();
    glTranslatef(0,-1.f,0);

    // Apply vertex color as material color.
    // Because the cube has colors set per vertex, we can color each face seperately
    glEnable(GL_COLOR_MATERIAL);

    // Apply to ambient and diffuse material properties
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    draw_cube();
    
    glDisable(GL_COLOR_MATERIAL);

    glPopMatrix();
}

#endif

#ifndef DECAL_H
#define DECAL_H

#include <libdragon.h>
#include <GL/gl.h>

void draw_quad()
{
    glBegin(GL_TRIANGLE_STRIP);
        glNormal3f(0, 1, 0);
        glTexCoord2f(0, 0);
        glVertex3f(-0.5f, 0, -0.5f);
        glTexCoord2f(0, 1);
        glVertex3f(-0.5f, 0, 0.5f);
        glTexCoord2f(1, 0);
        glVertex3f(0.5f, 0, -0.5f);
        glTexCoord2f(1, 1);
        glVertex3f(0.5f, 0, 0.5f);
    glEnd();
}

void render_decal()
{
    rdpq_debug_log_msg("Decal");
    glPushMatrix();
    glTranslatef(0, 0, 6);
    glRotatef(35, 0, 1, 0);
    glScalef(3, 3, 3);

    // Decals are drawn with the depth func set to GL_EQUAL. Note that glPolygonOffset is not supported on N64.
    glDepthFunc(GL_EQUAL);

    // Disable writing to depth buffer, because the depth value will be the same anyway
    glDepthMask(GL_FALSE);

    // Apply vertex color as material color.
    // This time, we set one vertex color for the entire model.
    glEnable(GL_COLOR_MATERIAL);
    glColor4f(1.0f, 0.4f, 0.2f, 0.5f);

    draw_quad();

    glDisable(GL_COLOR_MATERIAL);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    
    glPopMatrix();
}

#endif


#ifndef PLANE_H
#define PLANE_H

#include <libdragon.h>
#include <GL/gl.h>
#include <math.h>

#define PLANE_SIZE       20.0f
#define PLANE_SEGMENTS   16

static GLuint plane_buffers[2];
static GLuint plane_array;
static uint32_t plane_vertex_count;
static uint32_t plane_index_count;

void setup_plane()
{
    glGenBuffersARB(2, plane_buffers);

    glGenVertexArrays(1, &plane_array);
    glBindVertexArray(plane_array);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, plane_buffers[0]);

    glVertexPointer(3, GL_FLOAT, sizeof(vertex_t), (void*)(0*sizeof(float)));
    glTexCoordPointer(2, GL_FLOAT, sizeof(vertex_t), (void*)(3*sizeof(float)));
    glNormalPointer(GL_FLOAT, sizeof(vertex_t), (void*)(5*sizeof(float)));
    
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    glBindVertexArray(0);
}

void make_plane_mesh()
{
    plane_vertex_count = (PLANE_SEGMENTS + 1) * (PLANE_SEGMENTS + 1);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, plane_buffers[0]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, plane_vertex_count * sizeof(vertex_t), NULL, GL_STATIC_DRAW_ARB);

    const float p0 = - (PLANE_SIZE / 2);
    const float incr = PLANE_SIZE / PLANE_SEGMENTS;

    vertex_t *vertices = glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);

    for (uint32_t y = 0; y < PLANE_SEGMENTS + 1; y++)
    {
        for (uint32_t x = 0; x < PLANE_SEGMENTS + 1; x++)
        {
            uint32_t i = y * (PLANE_SEGMENTS + 1) + x;
            vertex_t *v = &vertices[i];

            v->position[0] = p0 + incr * x;
            v->position[1] = 0;
            v->position[2] = p0 + incr * y;

            v->normal[0] = 0;
            v->normal[1] = 1;
            v->normal[2] = 0;

            v->texcoord[0] = x;
            v->texcoord[1] = y;
        }
    }
    
    glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    plane_index_count = PLANE_SEGMENTS * PLANE_SEGMENTS * 6;

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, plane_buffers[1]);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, plane_index_count * sizeof(uint16_t), NULL, GL_STATIC_DRAW_ARB);

    uint16_t *indices = glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);

    for (uint32_t y = 0; y < PLANE_SEGMENTS; y++)
    {
        for (uint32_t x = 0; x < PLANE_SEGMENTS; x++)
        {
            uint32_t i = (y * PLANE_SEGMENTS + x) * 6;

            uint32_t row_start = y * (PLANE_SEGMENTS + 1);
            uint32_t next_row_start = (y + 1) * (PLANE_SEGMENTS + 1);

            indices[i + 0] = x + row_start;
            indices[i + 1] = x + next_row_start;
            indices[i + 2] = x + row_start + 1;
            indices[i + 3] = x + next_row_start;
            indices[i + 4] = x + next_row_start + 1;
            indices[i + 5] = x + row_start + 1;
        }
    }

    glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

void draw_plane()
{
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, plane_buffers[1]);
    glBindVertexArray(plane_array);

    glDrawElements(GL_TRIANGLES, plane_index_count, GL_UNSIGNED_SHORT, 0);

    glBindVertexArray(0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

void render_plane()
{
    rdpq_debug_log_msg("Plane");

    draw_plane();
}

#endif



#ifndef PRIM_TEST_H
#define PRIM_TEST_H

#include <libdragon.h>
#include <GL/gl.h>

void points()
{
    glBegin(GL_POINTS);
        glVertex2f(-1.f, -1.f);
        glVertex2f(1.f, -1.f);
        glVertex2f(1.f, 1.f);
        glVertex2f(-1.f, 1.f);
    glEnd();
}

void lines()
{
    glBegin(GL_LINES);
        glVertex2f(-1.f, -1.f);
        glVertex2f(1.f, -1.f);
        glVertex2f(-1.f, 0.f);
        glVertex2f(1.f, 0.f);
        glVertex2f(-1.f, 1.f);
        glVertex2f(1.f, 1.f);
    glEnd();
}

void line_strip()
{
    glBegin(GL_LINE_STRIP);
        glVertex2f(-1.f, -1.f);
        glVertex2f(1.f, -1.f);
        glVertex2f(1.f, 1.f);
        glVertex2f(-1.f, 1.f);
    glEnd();
}

void line_loop()
{
    glBegin(GL_LINE_LOOP);
        glVertex2f(-1.f, -1.f);
        glVertex2f(1.f, -1.f);
        glVertex2f(1.f, 1.f);
        glVertex2f(-1.f, 1.f);
    glEnd();
}

void triangles()
{
    glBegin(GL_TRIANGLES);
        glVertex2f(-1.f, -1.f);
        glVertex2f(0.f, -1.f);
        glVertex2f(-1.f, 0.f);

        glVertex2f(1.f, 1.f);
        glVertex2f(1.f, 0.f);
        glVertex2f(0.f, 1.f);
    glEnd();
}

void triangle_strip()
{
    glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(-1.f, -1.f);
        glVertex2f(1.f, -1.f);
        glVertex2f(-1.f, 1.f);
        glVertex2f(1.f, 1.f);
    glEnd();
}

void triangle_fan()
{
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.f, 0.f);
        glVertex2f(-1.f, 0.f);
        glVertex2f(0.f, -1.f);
        glVertex2f(1.f, 0.f);
        glVertex2f(0.f, 1.f);
        glVertex2f(-1.f, 0.f);
    glEnd();
}

void quads()
{
    glBegin(GL_QUADS);
        glVertex2f(-1.f, -1.f);
        glVertex2f(0.f, -1.f);
        glVertex2f(0.f, 0.f);
        glVertex2f(-1.f, 0.f);

        glVertex2f(1.f, 1.f);
        glVertex2f(0.f, 1.f);
        glVertex2f(0.f, 0.f);
        glVertex2f(1.f, 0.f);
    glEnd();
}

void quad_strip()
{
    glBegin(GL_QUAD_STRIP);
        glVertex2f(-1.f, -1.f);
        glVertex2f(1.f, -1.f);
        glVertex2f(-0.5f, 0.f);
        glVertex2f(0.5f, 0.f);
        glVertex2f(-1.f, 1.f);
        glVertex2f(1.f, 1.f);
    glEnd();
}

void polygon()
{
    glBegin(GL_POLYGON);
        glVertex2f(-1.f, 0.f);
        glVertex2f(-0.75f, -0.75f);
        glVertex2f(0.f, -1.f);
        glVertex2f(0.75f, -0.75f);
        glVertex2f(1.f, 0.f);
        glVertex2f(0.75f, 0.75f);
        glVertex2f(0.f, 1.f);
        glVertex2f(-0.75f, 0.75f);
    glEnd();
}

void prim_test()
{
    glPushMatrix();
    glTranslatef(-6, 1.5f, 0);
    points();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-3, 1.5f, 0);
    lines();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 1.5f, 0);
    line_strip();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3, 1.5f, 0);
    line_loop();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6, 1.5f, 0);
    triangles();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-6, -1.5f, 0);
    triangle_strip();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-3, -1.5f, 0);
    triangle_fan();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -1.5f, 0);
    quads();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3, -1.5f, 0);
    quad_strip();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6, -1.5f, 0);
    polygon();
    glPopMatrix();
}

void render_primitives(float rotation)
{
    rdpq_debug_log_msg("Primitives");
    glPushMatrix();

    glTranslatef(0, 6, 0);
    glRotatef(-rotation*2.46f, 0, 1, 0);

    // Configure alpha blending (transparency)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set a constant alpha for all vertices
    glColor4f(1, 1, 1, 0.4f);
    
    // We want to see back faces as well
    glDisable(GL_CULL_FACE);

    // Transparent polygons should not write to the depth buffer
    glDepthMask(GL_FALSE);

    prim_test();
    
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    glPopMatrix();
}

#endif

#ifndef SKINNED_H
#define SKINNED_H

#include <libdragon.h>
#include <GL/gl.h>
#include <stdint.h>

typedef struct {
    float position[3];
    float texcoord[2];
    float normal[3];
    uint8_t mtx_index;
} skinned_vertex_t;

static const skinned_vertex_t skinned_vertices[] = {
    { .position = {-2,  0, -1}, .texcoord = {0.f, 0.f}, .normal = { 0.f,  1.f,  0.f}, .mtx_index = 0 },
    { .position = {-2,  0,  1}, .texcoord = {1.f, 0.f}, .normal = { 0.f,  1.f,  0.f}, .mtx_index = 0 },
    { .position = {-1,  0, -1}, .texcoord = {0.f, 1.f}, .normal = { 0.f,  1.f,  0.f}, .mtx_index = 0 },
    { .position = {-1,  0,  1}, .texcoord = {1.f, 1.f}, .normal = { 0.f,  1.f,  0.f}, .mtx_index = 0 },
    { .position = { 1,  0, -1}, .texcoord = {0.f, 2.f}, .normal = { 0.f,  1.f,  0.f}, .mtx_index = 1 },
    { .position = { 1,  0,  1}, .texcoord = {1.f, 2.f}, .normal = { 0.f,  1.f,  0.f}, .mtx_index = 1 },
    { .position = { 2,  0, -1}, .texcoord = {0.f, 3.f}, .normal = { 0.f,  1.f,  0.f}, .mtx_index = 1 },
    { .position = { 2,  0,  1}, .texcoord = {1.f, 3.f}, .normal = { 0.f,  1.f,  0.f}, .mtx_index = 1 },
};

void draw_skinned()
{
    glEnable(GL_MATRIX_PALETTE_ARB);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_MATRIX_INDEX_ARRAY_ARB);

    glVertexPointer(        3,  GL_FLOAT,           sizeof(skinned_vertex_t),   skinned_vertices[0].position);
    glTexCoordPointer(      2,  GL_FLOAT,           sizeof(skinned_vertex_t),   skinned_vertices[0].texcoord);
    glNormalPointer(            GL_FLOAT,           sizeof(skinned_vertex_t),   skinned_vertices[0].normal);
    glMatrixIndexPointerARB(1,  GL_UNSIGNED_BYTE,   sizeof(skinned_vertex_t),   &skinned_vertices[0].mtx_index);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(skinned_vertices)/sizeof(skinned_vertex_t));

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_MATRIX_INDEX_ARRAY_ARB);

    glDisable(GL_MATRIX_PALETTE_ARB);
}

void render_skinned(const camera_t *camera, float animation)
{
    rdpq_debug_log_msg("Skinned");
    glPushMatrix();
    
    glTranslatef(0, 3, -6);
    glScalef(2, 2, 2);

    // Set bone transforms. Note that because there is no matrix stack in palette mode, we need
    // to apply the camera transform and model transform as well for each bone.
    glMatrixMode(GL_MATRIX_PALETTE_ARB);

    // Set transform of first bone
    glCurrentPaletteMatrixARB(0);
    glCopyMatrixN64(GL_MODELVIEW);
    glRotatef(sinf(animation*0.1f)*45, 0, 0, 1);

    // Set transform of second bone
    glCurrentPaletteMatrixARB(1);
    glCopyMatrixN64(GL_MODELVIEW);
    glRotatef(-sinf(animation*0.1f)*45, 0, 0, 1);

    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_CULL_FACE);
    draw_skinned();
    glEnable(GL_CULL_FACE);

    glPopMatrix();
}

#endif

#ifndef SPHERE_H
#define SPHERE_H

#include <libdragon.h>
#include <GL/gl.h>
#include <math.h>

#define SPHERE_RADIUS       20.0f
#define SPHERE_MIN_RINGS    4
#define SPHERE_MAX_RINGS    64
#define SPHERE_MIN_SEGMENTS 4
#define SPHERE_MAX_SEGMENTS 64

static GLuint sphere_buffers[2];
static GLuint sphere_array;
static GLuint sphere_list;
static uint32_t sphere_rings;
static uint32_t sphere_segments;
static uint32_t sphere_vertex_count;
static uint32_t sphere_index_count;

void setup_sphere()
{
    glGenBuffersARB(2, sphere_buffers);
    sphere_rings = 8;
    sphere_segments = 8;

    glGenVertexArrays(1, &sphere_array);
    glBindVertexArray(sphere_array);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, sphere_buffers[0]);

    glVertexPointer(3, GL_FLOAT, sizeof(vertex_t), (void*)(0*sizeof(float)));
    glTexCoordPointer(2, GL_FLOAT, sizeof(vertex_t), (void*)(3*sizeof(float)));
    glNormalPointer(GL_FLOAT, sizeof(vertex_t), (void*)(5*sizeof(float)));
    
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    glBindVertexArray(0);

    sphere_list = glGenLists(1);
}

void make_sphere_vertex(vertex_t *dst, uint32_t ring, uint32_t segment)
{
    float r = SPHERE_RADIUS;
    float phi = (M_TWOPI * segment) / sphere_segments;
    float theta = (M_PI * ring) / (sphere_rings + 1);

    float sintheta = sin(theta);

    float x = r * cosf(phi) * sintheta;
    float y = r * sinf(phi) * sintheta;
    float z = r * cosf(theta);

    dst->position[0] = x;
    dst->position[1] = y;
    dst->position[2] = z;

    float mag2 = x*x + y*y + z*z;
    float mag = sqrtf(mag2);
    float inv_m = 1.0f / mag;

    dst->normal[0] = -x * inv_m;
    dst->normal[1] = -y * inv_m;
    dst->normal[2] = -z * inv_m;

    dst->texcoord[0] = segment & 1 ? 1.0f : 0.0f;
    dst->texcoord[1] = ring & 1 ? 1.0f : 0.0f;
}

void draw_sphere_internal()
{
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sphere_buffers[1]);
    glBindVertexArray(sphere_array);

    glDrawElements(GL_TRIANGLE_FAN, sphere_segments + 2, GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_TRIANGLE_FAN, sphere_segments + 2, GL_UNSIGNED_SHORT, (void*)((sphere_segments + 2) * sizeof(uint16_t)));
    glDrawElements(GL_TRIANGLES, (sphere_rings - 1) * (sphere_segments * 6), GL_UNSIGNED_SHORT, (void*)((sphere_segments + 2) * 2 * sizeof(uint16_t)));

    glBindVertexArray(0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

void make_sphere_mesh()
{
    sphere_vertex_count = sphere_rings * sphere_segments + 2;

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, sphere_buffers[0]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sphere_vertex_count * sizeof(vertex_t), NULL, GL_STATIC_DRAW_ARB);

    vertex_t *vertices = glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);

    make_sphere_vertex(&vertices[0], 0, 0);
    
    for (uint32_t r = 0; r < sphere_rings; r++)
    {
        for (uint32_t s = 0; s < sphere_segments; s++)
        {
            make_sphere_vertex(&vertices[r * sphere_segments + s + 1], r + 1, s);
        }
    }

    make_sphere_vertex(&vertices[sphere_vertex_count - 1], sphere_rings + 1, 0);
    
    glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    uint32_t fan_index_count = sphere_segments + 2;
    uint32_t ring_index_count = sphere_segments * 6;

    sphere_index_count = fan_index_count * 2 + ring_index_count * (sphere_rings - 1);

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sphere_buffers[1]);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sphere_index_count * sizeof(uint16_t), NULL, GL_STATIC_DRAW_ARB);

    uint16_t *indices = glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);

    // Ends
    for (uint32_t i = 0; i < fan_index_count - 1; i++)
    {
        indices[i] = i;
        indices[fan_index_count + i] = sphere_vertex_count - i - 1;
    }
    indices[sphere_segments + 1] = 1;
    indices[fan_index_count + sphere_segments + 1] = sphere_vertex_count - 2;

    uint32_t rings_index_offset = fan_index_count * 2;

    // Rings
    for (uint32_t r = 0; r < sphere_rings - 1; r++)
    {
        uint16_t *ring_indices = &indices[rings_index_offset + r * ring_index_count];
        uint16_t first_ring_index = 1 + r * sphere_segments;
        uint16_t second_ring_index = 1 + (r + 1) * sphere_segments;

        for (uint32_t s = 0; s < sphere_segments; s++)
        {
            uint16_t next_segment = (s + 1) % sphere_segments;
            ring_indices[s * 6 + 0] = first_ring_index + s;
            ring_indices[s * 6 + 1] = second_ring_index + s;
            ring_indices[s * 6 + 2] = first_ring_index + next_segment;
            ring_indices[s * 6 + 3] = second_ring_index + s;
            ring_indices[s * 6 + 4] = second_ring_index + next_segment;
            ring_indices[s * 6 + 5] = first_ring_index + next_segment;
        }
    }

    glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

    glNewList(sphere_list, GL_COMPILE);
    draw_sphere_internal();
    glEndList();
}

void draw_sphere()
{
    // This is an example of using display lists
    glCallList(sphere_list);
}

void render_sphere(float rotation)
{
    rdpq_debug_log_msg("Sphere");
    glPushMatrix();

    glRotatef(rotation*0.23f, 1, 0, 0);
    glRotatef(rotation*0.98f, 0, 0, 1);
    glRotatef(rotation*1.71f, 0, 1, 0);

    // We want to see back faces instead of front faces, because the camera will be inside the sphere
    glCullFace(GL_FRONT);
    draw_sphere();
    glCullFace(GL_BACK);

    glPopMatrix();
}

#endif



#include <libdragon.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/gl_integration.h>
#include <rspq_profile.h>
#include <malloc.h>
#include <math.h>

// Set this to 1 to enable rdpq debug output.
// The demo will only run for a single frame and stop.
#define DEBUG_RDP 0

static uint32_t animation = 3283;
static uint32_t texture_index = 0;
static camera_t camera;
static surface_t zbuffer;

static uint64_t frames = 0;

static GLuint textures[4];

static GLenum shade_model = GL_SMOOTH;
static bool fog_enabled = false;

static const GLfloat environment_color[] = { 0.1f, 0.03f, 0.2f, 1.f };

static const GLfloat light_pos[8][4] = {
    { 1, 0, 0, 0 },
    { -1, 0, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, -1, 0 },
    { 8, 3, 0, 1 },
    { -8, 3, 0, 1 },
    { 0, 3, 8, 1 },
    { 0, 3, -8, 1 },
};

static const GLfloat light_diffuse[8][4] = {
    { 1.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 1.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 0.0f, 1.0f },
    { 1.0f, 0.0f, 1.0f, 1.0f },
    { 0.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
};


void setup()
{
    camera.distance = -10.0f;
    camera.rotation = 0.0f;

    zbuffer = surface_alloc(FMT_RGBA16, display_get_width(), display_get_height());

    for (uint32_t i = 0; i < 4; i++)
    {
        //sprites[i] = sprite_load(texture_path[i]);
    }

    setup_sphere();
    make_sphere_mesh();

    setup_cube();

    setup_plane();
    make_plane_mesh();

    float aspect_ratio = (float)display_get_width() / (float)display_get_height();
    float near_plane = 1.0f;
    float far_plane = 50.0f;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-near_plane*aspect_ratio, near_plane*aspect_ratio, -near_plane, near_plane, near_plane, far_plane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, environment_color);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    float light_radius = 10.0f;

    for (uint32_t i = 0; i < 8; i++)
    {
        glEnable(GL_LIGHT0 + i);
        glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light_diffuse[i]);
        glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 2.0f/light_radius);
        glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 1.0f/(light_radius*light_radius));
    }

    GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);

    glFogf(GL_FOG_START, 5);
    glFogf(GL_FOG_END, 20);
    glFogfv(GL_FOG_COLOR, environment_color);

    glEnable(GL_MULTISAMPLE_ARB);

    glGenTextures(4, textures);

    #if 0
    GLenum min_filter = GL_LINEAR_MIPMAP_LINEAR;
    #else
    GLenum min_filter = GL_LINEAR;
    #endif


    for (uint32_t i = 0; i < 4; i++)
    {
        glBindTexture(GL_TEXTURE_2D, textures[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);

        //glSpriteTextureN64(GL_TEXTURE_2D, sprites[i], &(rdpq_texparms_t){.s.repeats = REPEAT_INFINITE, .t.repeats = REPEAT_INFINITE});
    }
}

void set_light_positions(float rotation)
{
    glPushMatrix();
    glRotatef(rotation*5.43f, 0, 1, 0);

    for (uint32_t i = 0; i < 8; i++)
    {
        glLightfv(GL_LIGHT0 + i, GL_POSITION, light_pos[i]);
    }
    glPopMatrix();
}

void render()
{
    surface_t *disp = display_get();

    rdpq_attach(disp, &zbuffer);

    gl_context_begin();

    //glClearColor(environment_color[0], environment_color[1], environment_color[2], environment_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glMatrixMode(GL_MODELVIEW);
    //camera_transform(&camera);

    float rotation = animation * 0.5f;

    set_light_positions(rotation);

    // Set some global render modes that we want to apply to all models
    //glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, textures[texture_index]);
    
    //render_plane();
    //render_decal();
    //render_cube();
    game_draw(&game);
    //render_skinned(&camera, animation);

    //game_update(&game);
    //game_draw(&game);

    //glBindTexture(GL_TEXTURE_2D, textures[(texture_index + 1)%4]);
    //render_sphere(rotation);

    //glDisable(GL_TEXTURE_2D);
    //glDisable(GL_LIGHTING);
    //render_primitives(rotation);

    gl_context_end();

    rdpq_detach_show();

    rspq_profile_next_frame();

    if (((frames++) % 60) == 0) {
        rspq_profile_dump();
        rspq_profile_reset();
    }
}

int main()
{
	debug_init_isviewer();
	debug_init_usblog();
    dfs_init(DFS_DEFAULT_LOCATION);
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS_DEDITHER);
    rdpq_init();
    gl_init();

#if DEBUG_RDP
    rdpq_debug_start();
    rdpq_debug_log(true);
#endif

    setup();

    joypad_init();
    timer_init();

    rspq_profile_start();
    fw64_libdragon_engine_init(&engine);
    debugf("fw64_libdragon_engine_init\n");
    game_init(&game, &engine.interface);

#if !DEBUG_RDP
    while (1)
#endif
    {
        joypad_poll();
        joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        joypad_buttons_t held = joypad_get_buttons_held(JOYPAD_PORT_1);
        joypad_inputs_t inputs = joypad_get_inputs(JOYPAD_PORT_1);

        if (held.a) {
            animation++;
        }

        if (held.b) {
            animation--;
        }

        if (pressed.start) {
            debugf("%ld\n", animation);
        }

        if (pressed.r) {
            shade_model = shade_model == GL_SMOOTH ? GL_FLAT : GL_SMOOTH;
            glShadeModel(shade_model);
        }

        if (pressed.l) {
            fog_enabled = !fog_enabled;
            if (fog_enabled) {
                glEnable(GL_FOG);
            } else {
                glDisable(GL_FOG);
            }
        }

        if (pressed.c_up) {
            if (sphere_rings < SPHERE_MAX_RINGS) {
                sphere_rings++;
            }

            if (sphere_segments < SPHERE_MAX_SEGMENTS) {
                sphere_segments++;
            }

            make_sphere_mesh();
        }

        if (pressed.c_down) {
            if (sphere_rings > SPHERE_MIN_RINGS) {
                sphere_rings--;
            }

            if (sphere_segments > SPHERE_MIN_SEGMENTS) {
                sphere_segments--;
            }
            
            make_sphere_mesh();
        }

        if (pressed.c_right) {
            texture_index = (texture_index + 1) % 4;
        }

        float y = inputs.stick_y / 128.f;
        float x = inputs.stick_x / 128.f;
        float mag = x*x + y*y;

        if (fabsf(mag) > 0.01f) {
            camera.distance += y * 0.2f;
            camera.rotation = camera.rotation - x * 1.2f;
        }

        fw64_libdragon_engine_update(&engine);
        game_update(&game);
        render();
        if (DEBUG_RDP)
            rspq_wait();
    }

}