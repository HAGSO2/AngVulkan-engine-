#include <core/logger.h>
#include <game_types.h>
#include <platform/platform.h>

Game::Game() 
{
    state = platform_allocate(sizeof(game_state), FALSE);
};

b8 Game::initialize(Game* game_inst) {
    KDEBUG("game_initialize() called!");
    return TRUE;
}

b8 Game::update(Game* game_inst, f32 delta_time) {
    return TRUE;
}

b8 Game::render(Game* game_inst, f32 delta_time) {
    return TRUE;
}

void Game::on_resize(Game* game_inst, u32 width, u32 height) {
}