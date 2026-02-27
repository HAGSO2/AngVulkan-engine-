#pragma once

#include "defines.h"
#include "config/renderer_options.h"

struct game;

// Application configuration.
typedef struct application_config{
    // Window starting position x axis, if applicable.
    i16 start_pos_x;

    // Window starting position y axis, if applicable.
    i16 start_pos_y;

    // Window starting width, if applicable.
    i16 start_width;

    // Window starting height, if applicable.
    i16 start_height;

    // The application name used in windowing , if applicable.
    char* name;

    //The name of the engine
    char* engine;

    //Renderer options
    vulkan_options* vlk_opt;
} application_config;

KAPI b8 application_create(struct game* game_inst);

KAPI b8 application_run();

void application_get_framebuffer_size(u32* width, u32* height);