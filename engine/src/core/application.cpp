#include "application.h"
#include "game_types.h"
#include "logger.h"
#include "platform/platform.h"

application_state::application_state(Game* g): game_inst{g},is_running{FALSE}, is_suspended{FALSE}, platform{0}, width{0}, height{0}, last_time{0}{};

application_config::application_config(i16 m_start_pos_x,i16 m_start_pos_y,i16 m_start_width,i16 m_start_height, string m_name):
start_pos_x{m_start_pos_x}, start_pos_y{m_start_pos_y},start_width{m_start_width}, start_height{m_start_height}, name{m_name} {};

Application::Application(i16 start_pos_x,i16 start_pos_y,i16 start_width,i16 start_height, string name):
app_config(start_pos_x,start_pos_y,start_width,start_height, name), initialized{FALSE}, app_state{0} {};

b8 Application::application_create(Game* game_inst) {
    if (initialized) {
        KERROR("application_create called more than once.");
        return FALSE;
    }

    app_state.game_inst = game_inst;

    // Initialize subsystems.
    initialize_logging();

    // TODO: Remove this
    KFATAL("A test message: %f", 3.14f);
    KERROR("A test message: %f", 3.14f);
    KWARN("A test message: %f", 3.14f);
    KINFO("A test message: %f", 3.14f);
    KDEBUG("A test message: %f", 3.14f);
    KTRACE("A test message: %f", 3.14f);

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    if (!platform_startup(
            &app_state.platform,
            app_config.name.c_str(),
            app_config.start_pos_x,
            app_config.start_pos_y,
            app_config.start_width,
            app_config.start_height)) {
        return FALSE;
    }

    // Initialize the game.
    if (!app_state.game_inst->initialize(app_state.game_inst)) {
        KFATAL("Game failed to initialize.");
        return FALSE;
    }

    app_state.game_inst->on_resize(app_state.game_inst, app_state.width, app_state.height);

    initialized = TRUE;
    
    return TRUE;
}

b8 Application::application_run() {
    while (app_state.is_running) {
        if(!platform_pump_messages(&app_state.platform)) {
            app_state.is_running = FALSE;
        }

        if(!app_state.is_suspended) {
            if (!app_state.game_inst->update(app_state.game_inst, (f32)0)) {
                KFATAL("Game update failed, shutting down.");
                app_state.is_running = FALSE;
                break;
            }

            // Call the game's render routine.
            if (!app_state.game_inst->render(app_state.game_inst, (f32)0)) {
                KFATAL("Game render failed, shutting down.");
                app_state.is_running = FALSE;
                break;
            }
        }
    }

    app_state.is_running = FALSE;

    platform_shutdown(&app_state.platform);

    return TRUE;
}