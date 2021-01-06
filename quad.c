#include "assets.h"

#include <nusys.h>

const Vtx quad_vtx[] =  {
        {        -32,  32, -5, 0, 0, 0, 0, 0xff, 0, 0xff	},
        {         32,  32, -5, 0, 0, 0, 0, 0, 0, 0xff	},
        {         32, -32, -5, 0, 0, 0, 0, 0, 0xff, 0xff	},
        {        -32, -32, -5, 0, 0, 0, 0xff, 0, 0, 0xff	},
};

const int quad_vtx_count = 4;

const Gfx quad_display_list[] = {
  gsSPVertex(&(quad_vtx[0]),quad_vtx_count, 0),
  gsSP2Triangles(0,1,2,0,0,2,3,0),
  gsSPEndDisplayList()
};