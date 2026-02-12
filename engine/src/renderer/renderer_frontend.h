#pragma once

#include "renderer_types.inl"
#include "core/application.h"

struct static_mesh_data;
struct platform_state;

b8 renderer_initialize(const char* application_name, const char* engine_name, struct platform_state* plat_state, Application* app);
void renderer_shutdown();

void renderer_on_resized(u16 width, u16 height);

b8 renderer_draw_frame(render_packet* packet);