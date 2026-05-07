#include "renderer_frontend.h"

#include "renderer_backend.h"

#include "core/logger.h"
#include "math/kmath.h"
#include "math/math_types.h"

typedef struct renderer_system_state {
    renderer_backend backend;
    mat4 projection;
    mat4 view;
    f32 near_clip;
    f32 far_clip;
} renderer_system_state;

// Backend render context.
static renderer_system_state* state_ptr = 0;

b8 renderer_initialize(const char* application_name, const char* engine_name, struct platform_state* plat_state, vulkan_options* opt) {
    state_ptr = new renderer_system_state{0};

    // TODO: make this configurable.
    renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, plat_state,&state_ptr->backend);
    state_ptr->backend.frame_number = 0;

    if (!state_ptr->backend.initialize(&state_ptr->backend, application_name, engine_name, plat_state, opt)) {
        KFATAL("Renderer backend failed to initialize. Shutting down.");
        return FALSE;
    }

    state_ptr->near_clip = 0.1f;
    state_ptr->far_clip = 1000.0f;
    state_ptr->projection = mat4::perspective(deg_to_rad(45.0f), 1280 / 720.0f, state_ptr->near_clip, state_ptr->far_clip);

    state_ptr->view = mat4::translation((vec3){0, 0, -30.0f});
    state_ptr->view = state_ptr->view.inverse();

    return TRUE;
}

void renderer_shutdown() {
    if (state_ptr) {
        state_ptr->backend.shutdown(&state_ptr->backend);
    }
    state_ptr = 0;
}

b8 renderer_begin_frame(f32 delta_time) {
    if (!state_ptr) {
        return false;
    }
    return state_ptr->backend.begin_frame(&state_ptr->backend, delta_time);
}

b8 renderer_end_frame(f32 delta_time) {
    if (!state_ptr) {
        return false;
    }
    b8 result = state_ptr->backend.end_frame(&state_ptr->backend, delta_time);
    state_ptr->backend.frame_number++;
    return result;
}

void renderer_on_resized(u16 width, u16 height) {
    if (state_ptr) {
        state_ptr->projection = mat4::perspective(deg_to_rad(45.0f), width / (f32)height, state_ptr->near_clip, state_ptr->far_clip);
        state_ptr->backend.resized(&state_ptr->backend, width, height);
    } else {
        KWARN("renderer backend does not exist to accept resize: %i %i", width, height);
    }
}

b8 renderer_draw_frame(render_packet* packet) {
    // If the begin frame returned successfully, mid-frame operations may continue.
    if (renderer_begin_frame(packet->delta_time)) {
        state_ptr->backend.update_global_state(state_ptr->projection, state_ptr->view, vec3::zero(), vec4::one(), 0);

        // mat4 model = mat4_translation((vec3){0, 0, 0});
        static f32 angle = 0.01f;
        angle += 0.001f;
        quat rotation = quat::from_axis_angle(vec3::forward(), angle, false);
        mat4 model = to_rotation_mat4(rotation, vec3::zero());
        state_ptr->backend.update_object(model);

        // End the frame. If this fails, it is likely unrecoverable.
        b8 result = renderer_end_frame(packet->delta_time);

        if (!result) {
            KERROR("renderer_end_frame failed. Application shutting down...");
            return false;
        }
    }

    return TRUE;
}

void renderer_set_view(mat4 view) {
    state_ptr->view = view;
}