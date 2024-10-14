#include "libdragon.h"
#include <malloc.h>
#include <math.h>

#include <framework64/allocator.h>
#include <framework64/controller_mapping/n64.h>
#include <framework64/n64_libdragon/libdragon_engine.h>
#include <framework64/n64_libdragon/libdragon_texture.h>
#include <framework64/util/texture_util.h>

//Animation frame size defines
#define ANIM_FRAME_W 120
#define ANIM_FRAME_H 80

//Animation frame timing defines
#define ANIM_FRAME_DELAY 3
#define ANIM_FRAME_MAX 6

#define KNIGHT_MAX 2

//Structure for knight sprite
typedef struct {
    float x; 
    float y;
    bool attack;
    bool flip;
    int time;
} knight_data;

static knight_data knights[2];
static sprite_t *sheet_knight;

fw64Texture* knight_texture;
fw64RenderPass* renderpass;
fw64SpriteBatch* spritebatch;

fw64Engine engine;

void render(void)
{
    fw64_renderer_begin(engine.renderer, FW64_PRIMITIVE_MODE_TRIANGLES, FW64_CLEAR_FLAG_ALL );


    fw64_spritebatch_begin(spritebatch);

    for(int i=0; i < KNIGHT_MAX; i++) {
        int frame = knights[i].time/ANIM_FRAME_DELAY; //Calculate knight frame
        fw64_spritebatch_draw_sprite_slice(spritebatch, knight_texture, frame, knights[i].x, knights[i].y);
    }
    fw64_spritebatch_end(spritebatch);
    fw64_renderpass_begin(renderpass);
    fw64_renderpass_draw_sprite_batch(renderpass, spritebatch);
    fw64_renderpass_end(renderpass);

    fw64_renderer_submit_renderpass(engine.renderer, renderpass);
    fw64_renderer_end(engine.renderer, FW64_RENDERER_FLAG_SWAP);
    
}

void update(void)
{
    //Set attack for left knight
    if (fw64_input_controller_button_pressed(engine.input, 0, FW64_N64_CONTROLLER_BUTTON_A)) {
        knights[0].attack = true;        
    }
    //Set attack for right knight
    if(fw64_input_controller_button_pressed(engine.input, 0, FW64_N64_CONTROLLER_BUTTON_B)) {
        knights[1].attack = true;
    }

    if(fw64_input_controller_button_pressed(engine.input, 0, FW64_N64_CONTROLLER_BUTTON_START)) {
        fw64_texture_util_create_from_loaded_image(NULL, "rom:/0", fw64_default_allocator());
    }

    for(int i=0; i<KNIGHT_MAX; i++) {
        if(knights[i].attack) {
            knights[i].time++; //Increment time
            //Stop attack at end of animation
            if(knights[i].time >= ANIM_FRAME_DELAY*ANIM_FRAME_MAX) {
                knights[i].time = 0;
                knights[i].attack = false;  
            }
        }
    }
}

int main()
{
    fw64_n64_libdragon_engine_init(&engine);

    fw64Display* display = fw64_displays_get_primary(engine.displays);
    fw64Allocator* allocator = fw64_default_allocator();

    renderpass = fw64_renderpass_create(display, allocator);
    spritebatch = fw64_spritebatch_create(1, allocator);
    
    knight_texture = fw64_texture_util_create_from_loaded_image(NULL, "rom:/0", allocator);
    sheet_knight = knight_texture->image->libdragon_sprite;
    IVec2 display_size = fw64_display_get_size(display);
    //Initialize left knight
    knights[0].x = (display_size.x/2)-25;
    knights[0].y = display_size.y-30;
    //Initialize right knight
    knights[1].x = (display_size.x/2)+25;
    knights[1].y = knights[0].y;
    knights[1].flip = true;   
    while (1)
    {
        
        fw64_n64_libdragon_engine_update(&engine);
        update();
        render();
    }
}