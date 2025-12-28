#include "application.h"
#include "logger.h"

#include "core/event.h"
#include "core/input.h"

#include "renderer/renderer_frontend.h"

application_state::application_state(Game* g): game_inst{g},is_running{FALSE}, is_suspended{FALSE}, platform{0}, width{0}, height{0}, last_time{0}{};

application_config::application_config(i16 m_start_pos_x,i16 m_start_pos_y,i16 m_start_width,i16 m_start_height, string m_name, string m_engine):
start_pos_x{m_start_pos_x}, start_pos_y{m_start_pos_y},start_width{m_start_width}, start_height{m_start_height}, name{m_name}, engine{m_engine} {};

Application::Application(i16 start_pos_x,i16 start_pos_y,i16 start_width,i16 start_height, string name, string engine, vulkan_options* opt):
app_config(start_pos_x,start_pos_y,start_width,start_height, name, engine), initialized{FALSE}, vlk_opt{opt} , app_state{0} {};

// Event handlers
b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context);
b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context);

b8 Application::application_create(Game* game_inst) {
    if (initialized) {
        KERROR("application_create called more than once.");
        return FALSE;
    }

    app_state.game_inst = game_inst;

    // Initialize subsystems.
    initialize_logging();
    input_initialize();

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    if(!event_initialize()) {
        KERROR("Event system failed initialization. Application cannot continue.");
        return FALSE;
    }

    event_register(EVENT_CODE_APPLICATION_QUIT, this, application_on_event);
    event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

    if (!platform_startup(
            &app_state.platform,
            app_config.name.c_str(),
            app_config.start_pos_x,
            app_config.start_pos_y,
            app_config.start_width,
            app_config.start_height)) {
        return FALSE;
    }

    // Renderer startup
    if (!renderer_initialize(app_config.name.c_str(), app_config.engine.c_str(), &app_state.platform, vlk_opt)) {
        KFATAL("Failed to initialize renderer. Aborting application.");
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

            // NOTE: Input update/state copying should always be handled
            // after any input should be recorded; I.E. before this line.
            // As a safety, input is the last thing to be updated before
            // this frame ends.
            input_update(0);
        }
    }

    app_state.is_running = FALSE;
    
    // Shutdown event system.
    event_unregister(EVENT_CODE_APPLICATION_QUIT, this, application_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    event_shutdown();
    input_shutdown();
    platform_shutdown(&app_state.platform);

    return TRUE;
}

b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context) {
    KDEBUG("Event quit called!");
    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT: {
            KINFO("EVENT_CODE_APPLICATION_QUIT recieved, shutting down.\n");
            Application* app = (Application*) listener_inst;
            app->GetState()->is_running = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}

b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context) {
    KDEBUG("Event key called!");
    if (code == EVENT_CODE_KEY_PRESSED) {
        u16 key_code = context.data.u16[0];
        if (key_code == KEY_ESCAPE) {
            // NOTE: Technically firing an event to itself, but there may be other listeners.
            event_context data = {};
            event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);

            // Block anything else from processing this.
            return TRUE;
        } else if (key_code == KEY_A) {
            // Example on checking for a key
            KDEBUG("Explicit - A key pressed!");
        } else {
            KDEBUG("'%c' key pressed in window.", key_code);
        }
    } else if (code == EVENT_CODE_KEY_RELEASED) {
        u16 key_code = context.data.u16[0];
        if (key_code == KEY_B) {
            // Example on checking for a key
            KDEBUG("Explicit - B key released!");
        } else {
            KDEBUG("'%c' key released in window.", key_code);
        }
    }
    return FALSE;
}