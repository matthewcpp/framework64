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

fw64Engine engine;

void render(void)
{
    // Attach and clear the screen
    surface_t *disp = display_get();
    rdpq_attach_clear(disp, NULL);
    //Set render mode to the standard render mode
    rdpq_set_mode_standard();
    rdpq_mode_alphacompare(1); // colorkey (draw pixel with alpha >= 1)
    for(int i=0; i<KNIGHT_MAX; i++) {
        int frame = knights[i].time/ANIM_FRAME_DELAY; //Calculate knight frame
        //Draw knight sprite
        rdpq_sprite_blit(sheet_knight, knights[i].x, knights[i].y, &(rdpq_blitparms_t){
            .s0 = frame*ANIM_FRAME_W, //Extract correct sprite from sheet
            //Set sprite center to bottom-center
            .cx = ANIM_FRAME_W/2,
            .cy = ANIM_FRAME_H,
            .width = ANIM_FRAME_W, //Extract correct width from sheet
            .flip_x = knights[i].flip
        });
    }
    //Detach the screen
    rdpq_detach_show();
}

void update(void)
{
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
    
    fw64Texture* texture = fw64_texture_util_create_from_loaded_image(engine.assets, "rom:/0", fw64_default_allocator());
    sheet_knight = texture->image->libdragon_sprite;   
    //Initialize left knight
    knights[0].x = (display_get_width()/2)-25;
    knights[0].y = display_get_height()-30;
    //Initialize right knight
    knights[1].x = (display_get_width()/2)+25;
    knights[1].y = knights[0].y;
    knights[1].flip = true;
    while (1)
    {
        render();
        update();
        
        fw64_n64_libdragon_engine_update(&engine);
        //Set attack for left knight
        if (fw64_input_controller_button_pressed(engine.input, 0, FW64_N64_CONTROLLER_BUTTON_A)) {
            knights[0].attack = true;        
        }
        //Set attack for right knight
        if(fw64_input_controller_button_pressed(engine.input, 0, FW64_N64_CONTROLLER_BUTTON_B)) {
            knights[1].attack = true;
        }
    }
}