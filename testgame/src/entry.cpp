#include "game_types.h"
#include "core/application.h"
#include "core/logger.h"

/**
 * The main entry point of the application.
 */

int main(void){
    // Request the game instance from the application.
    Game game_inst = Game();
    Application app = Application(100,100,1280,720, "Angel C++ Vulkan Engine");

    // Initialization.
    if (!app.application_create(&game_inst)) {
        KINFO("Application failed to create!.");
        return 1;
    }

    // Begin the game loop.
    if(!app.application_run()) {
        KINFO("Application did not shutdown gracefully.");
        return 2;
    }

    return 0;
}