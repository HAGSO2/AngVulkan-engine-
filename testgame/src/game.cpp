#include <core/logger.h>
#include <game_types.h>
#include <platform/platform.h>
#include "game.h"

b8 game_initialize(game* game_inst){
    KDEBUG("game_initialize() called!");

    game_state* state = (game_state*)game_inst->state;
    
    return TRUE;
}

b8 game_update(game* game_inst, f32 delta_time){
    return TRUE;
}

b8 game_render(game* game_inst, f32 delta_time){
    return TRUE;
}

void game_on_resize(game* game_inst, u32 width, u32 heigth){
}