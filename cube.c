#include "cube.h"

#include <nusys.h>

const Vtx cube_vtx[] =  {
        // front
        {        -5,  5, 5, 0, 0, 0, 0, 0xff, 0, 0xff	},
        {         5,  5, 5, 0, 0, 0, 0, 0xff, 0, 0xff	},
        {         5, -5, 5, 0, 0, 0, 0, 0xff, 0, 0xff	},
        {        -5, -5, 5, 0, 0, 0, 0, 0xff, 0, 0xff	},

        //back
        {        -5,  5, -5, 0, 0, 0, 0xff, 0, 0, 0xff	},
        {         5,  5, -5, 0, 0, 0, 0xff, 0, 0, 0xff	},
        {         5, -5, -5, 0, 0, 0, 0xff, 0, 0, 0xff	},
        {        -5, -5, -5, 0, 0, 0, 0xff, 0, 0, 0xff	},

        //left
        {        -5,  5, -5, 0, 0, 0, 0xff, 0xff, 0, 0xff},
        {        -5,  5, 5, 0, 0, 0, 0xff, 0xff, 0, 0xff},
        {        -5,  -5, 5, 0, 0, 0, 0xff, 0xff, 0, 0xff},
        {        -5,  -5, -5, 0, 0, 0, 0xff, 0xff, 0, 0xff},

        //right
        {        5,  5, 5, 0, 0, 0, 0, 0, 0xff, 0xff},
        {        5,  5, -5, 0, 0, 0, 0, 0, 0xff, 0xff},
        {        5,  -5, -5, 0, 0, 0, 0, 0, 0xff, 0xff},
        {        5,  -5, 5, 0, 0, 0, 0, 0, 0xff, 0xff},

        //bottom
        {        -5,  -5, -5, 0, 0, 0, 0xff, 0xff, 0xff, 0xff},
        {        5,  -5, -5, 0, 0, 0, 0xff, 0xff, 0xff, 0xff},
        {        5,  -5, 5, 0, 0, 0, 0xff, 0xff, 0xff, 0xff},
        {        -5,  -5, 5, 0, 0, 0, 0xff, 0xff, 0xff, 0xff},

        //top
        {        -5,  5, -5, 0, 0, 0, 0xff, 0, 0xff, 0xff},
        {        5,  5, -5, 0, 0, 0, 0xff, 0, 0xff, 0xff},
        {        5,  5, 5, 0, 0, 0, 0xff, 0, 0xff, 0xff},
        {        -5,  5, 5, 0, 0, 0, 0xff, 0, 0xff, 0xff},


};

const int cube_vtx_count = 24;

const Gfx cube_display_list[] = {
  gsSPVertex(&(cube_vtx[0]),cube_vtx_count, 0),
  gsSP2Triangles(0,1,2,0,0,2,3,0),
  gsSP2Triangles(4,5,6,0,4,6,7,0),
  gsSP2Triangles(8,9,10,0,8,10,11,0),
  gsSP2Triangles(12,13,14,0,12,14,15,0),
  gsSP2Triangles(16,17,18,0,16,18,19,0),
  gsSP2Triangles(20,21,22,0,20,22,23,0),
  gsSPEndDisplayList()
};
 
const float cube_bounding[] = {-5.0f, -5.0f, -5.0f, 5.0f, 5.0f, 5.0f};