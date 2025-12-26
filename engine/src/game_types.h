#pragma once

#include "defines.h"
#include <string>
using namespace std;
/**
 * Represents the basic game state in a game.
 * Called for creation by the application.
 */

 //TODO: En vez de ser una clase debería ser una interfaz
class Game {
public:
    Game();
    // Function pointer to game's initialize function.
    b8 initialize(struct Game* game_inst);

    // Function pointer to game's update function.
    b8 update(struct Game* game_inst, f32 delta_time);

    // Function pointer to game's render function.
    b8 render(struct Game* game_inst, f32 delta_time);

    // Function pointer to handle resizes, if applicable.
    void on_resize(struct Game* game_inst, u32 width, u32 height);

    // Game-specific game state. Created and managed by the game.
    void* state;
};

typedef struct game_state {
    f32 delta_time;
} game_state;