#include "object.h"

const Vtx blue_cube_wire_mesh_0_verts[] = {
{ 1, 1, 1, 0, 0, 0, 0, 0, 0, 255 },
{ 1, 1, -1, 0, 0, 0, 0, 0, 0, 255 },
{ -1, 1, -1, 0, 0, 0, 0, 0, 0, 255 },
{ -1, 1, 1, 0, 0, 0, 0, 0, 0, 255 },
{ -1, 1, 1, 0, 0, 0, 0, 0, 0, 255 },
{ -1, -1, 1, 0, 0, 0, 0, 0, 0, 255 },
{ 1, -1, 1, 0, 0, 0, 0, 0, 0, 255 },
{ 1, 1, 1, 0, 0, 0, 0, 0, 0, 255 },
{ -1, 1, -1, 0, 0, 0, 0, 0, 0, 255 },
{ -1, -1, -1, 0, 0, 0, 0, 0, 0, 255 },
{ -1, -1, 1, 0, 0, 0, 0, 0, 0, 255 },
{ -1, 1, 1, 0, 0, 0, 0, 0, 0, 255 },
{ -1, -1, 1, 0, 0, 0, 0, 0, 0, 255 },
{ -1, -1, -1, 0, 0, 0, 0, 0, 0, 255 },
{ 1, -1, -1, 0, 0, 0, 0, 0, 0, 255 },
{ 1, -1, 1, 0, 0, 0, 0, 0, 0, 255 },
{ 1, 1, 1, 0, 0, 0, 0, 0, 0, 255 },
{ 1, -1, 1, 0, 0, 0, 0, 0, 0, 255 },
{ 1, -1, -1, 0, 0, 0, 0, 0, 0, 255 },
{ 1, 1, -1, 0, 0, 0, 0, 0, 0, 255 },
{ 1, 1, -1, 0, 0, 0, 0, 0, 0, 255 },
{ 1, -1, -1, 0, 0, 0, 0, 0, 0, 255 },
{ -1, -1, -1, 0, 0, 0, 0, 0, 0, 255 },
{ -1, 1, -1, 0, 0, 0, 0, 0, 0, 255 },
};

const Gfx blue_cube_wire_mesh_0_dl[] = {
gsSPVertex(blue_cube_wire_mesh_0_verts + 0, 24, 0),
gsSPLine3D(0, 1, 0),
gsSPLine3D(1, 2, 0),
gsSPLine3D(2, 0, 0),
gsSPLine3D(2, 3, 0),
gsSPLine3D(3, 0, 0),
gsSPLine3D(4, 5, 0),
gsSPLine3D(5, 6, 0),
gsSPLine3D(6, 4, 0),
gsSPLine3D(6, 7, 0),
gsSPLine3D(8, 9, 0),
gsSPLine3D(9, 10, 0),
gsSPLine3D(10, 8, 0),
gsSPLine3D(13, 14, 0),
gsSPLine3D(14, 12, 0),
gsSPLine3D(14, 15, 0),
gsSPLine3D(18, 16, 0),
gsSPLine3D(18, 19, 0),
gsSPLine3D(22, 20, 0),
gsSPEndDisplayList()
};

void wire_object_draw(Entity* entity, Renderer* renderer) {
    renderer_entity_start(renderer, entity);
    gSPDisplayList(renderer->display_list++, blue_cube_wire_mesh_0_dl);

    renderer_entity_end(renderer);
}

Lights1 blue_cube_lights_0 = gdSPDefLights1(6, 104, 204, 125, 125, 125, 40, 40, 40);

const Vtx blue_cube_mesh_0[] = {
{ 1, 1, 1, 0, 0, 0, 0, 127, 0, 255 },
{ 1, 1, -1, 0, 0, 0, 0, 127, 0, 255 },
{ -1, 1, -1, 0, 0, 0, 0, 127, 0, 255 },
{ -1, 1, 1, 0, 0, 0, 0, 127, 0, 255 },
{ -1, 1, 1, 0, 0, 0, 0, 0, 127, 255 },
{ -1, -1, 1, 0, 0, 0, 0, 0, 127, 255 },
{ 1, -1, 1, 0, 0, 0, 0, 0, 127, 255 },
{ 1, 1, 1, 0, 0, 0, 0, 0, 127, 255 },
{ -1, 1, -1, 0, 0, 0, -128, 0, 0, 255 },
{ -1, -1, -1, 0, 0, 0, -128, 0, 0, 255 },
{ -1, -1, 1, 0, 0, 0, -128, 0, 0, 255 },
{ -1, 1, 1, 0, 0, 0, -128, 0, 0, 255 },
{ -1, -1, 1, 0, 0, 0, 0, -128, 0, 255 },
{ -1, -1, -1, 0, 0, 0, 0, -128, 0, 255 },
{ 1, -1, -1, 0, 0, 0, 0, -128, 0, 255 },
{ 1, -1, 1, 0, 0, 0, 0, -128, 0, 255 },
{ 1, 1, 1, 0, 0, 0, 127, 0, 0, 255 },
{ 1, -1, 1, 0, 0, 0, 127, 0, 0, 255 },
{ 1, -1, -1, 0, 0, 0, 127, 0, 0, 255 },
{ 1, 1, -1, 0, 0, 0, 127, 0, 0, 255 },
{ 1, 1, -1, 0, 0, 0, 0, 0, -128, 255 },
{ 1, -1, -1, 0, 0, 0, 0, 0, -128, 255 },
{ -1, -1, -1, 0, 0, 0, 0, 0, -128, 255 },
{ -1, 1, -1, 0, 0, 0, 0, 0, -128, 255 },
};

const Gfx blue_cube_mesh_0_dl[] = {
gsSPSetLights1(blue_cube_lights_0),
gsSPVertex(blue_cube_mesh_0 + 0, 24, 0),
gsSP1Triangle(0, 1, 2, 0),
gsSP1Triangle(0, 2, 3, 0),
gsSP1Triangle(4, 5, 6, 0),
gsSP1Triangle(4, 6, 7, 0),
gsSP1Triangle(8, 9, 10, 0),
gsSP1Triangle(8, 10, 11, 0),
gsSP1Triangle(12, 13, 14, 0),
gsSP1Triangle(12, 14, 15, 0),
gsSP1Triangle(16, 17, 18, 0),
gsSP1Triangle(16, 18, 19, 0),
gsSP1Triangle(20, 21, 22, 0),
gsSP1Triangle(20, 22, 23, 0),
gsSPEndDisplayList()
};

void solid_object_draw(Entity* entity, Renderer* renderer) {
    renderer_entity_start(renderer, entity);
    gSPSetGeometryMode(renderer->display_list++, G_LIGHTING)
    gDPSetCombineMode(renderer->display_list++, G_CC_SHADE, G_CC_SHADE);
    gSPDisplayList(renderer->display_list++, blue_cube_mesh_0_dl);

    renderer_entity_end(renderer);
}

#include "Consolas12.h"

void init_consolas(Font* font) {
    font->glyph_count = Consolas12_FONT_GLYPH_COUNT;
    font->glyphs = &Consolas12_glyphs[0];
    font->size = Consolas12_FONT_SIZE;

    font->spritefont = &Consolas12_spritefont[0];
    font->spritefont_tile_width = Consolas12_SPRITEFONT_TILE_WIDTH;
    font->spritefont_tile_height = Consolas12_SPRITEFONT_TILE_HEIGHT;
}