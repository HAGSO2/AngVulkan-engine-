#pragma once

#include "defines.h"
#include "game_types.h"
#include "platform/platform.h"
#include <string>
using namespace std;

typedef struct application_state {
    Game* game_inst;
    b8 is_running;
    b8 is_suspended;
    platform_state platform;
    i16 width;
    i16 height;
    f64 last_time;
    application_state(Game* instance);
} application_state;

typedef struct application_config {
        // Window starting position x axis, if applicable.
        i16 start_pos_x;

        // Window starting position y axis, if applicable.
        i16 start_pos_y;

        // Window starting width, if applicable.
        i16 start_width;

        // Window starting height, if applicable.
        i16 start_height;

        // The application name used in windowing, if applicable.
        string name;
        application_config(i16 m_start_pos_x,i16 m_start_pos_y,i16 m_start_width,i16 m_start_height, string m_name);
    } application_config;

class Application{
    b8 initialized;

    application_config app_config;
    application_state app_state;

public:
    
    
    Application(i16 start_pos_x,i16 start_pos_y,i16 start_width,i16 start_height, string name);

    b8 application_create(Game* game_inst);

    b8 application_run();

    application_state* GetState() {return &app_state;}
};