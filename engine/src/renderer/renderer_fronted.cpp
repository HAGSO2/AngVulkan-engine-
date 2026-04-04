#include "renderer_frontend.h"

#include "renderer_backend.h"

#include "core/logger.h"
#include "math/kmath.h"
#include "math/math_types.h"

// Backend render context.
static renderer_backend* backend = 0;

b8 renderer_initialize(const char* application_name, const char* engine_name, struct platform_state* plat_state, vulkan_options* opt) {
    backend = new renderer_backend{0};

    // TODO: make this configurable.
    renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, plat_state, backend);
    backend->frame_number = 0;

    if (!backend->initialize(backend, application_name, engine_name, plat_state, opt)) {
        KFATAL("Renderer backend failed to initialize. Shutting down.");
        return FALSE;
    }

    return TRUE;
}

void renderer_shutdown() {
    if (backend) {
        backend->shutdown(backend);
        delete backend;
    }
}

b8 renderer_begin_frame(f32 delta_time) {
    if(!backend){
        return FALSE;
    }
    return backend->begin_frame(backend, delta_time);
}

b8 renderer_end_frame(f32 delta_time) {
    if(!backend){
        return FALSE;
    }
    b8 result = backend->end_frame(backend, delta_time);
    backend->frame_number++;
    return result;
}

void renderer_on_resized(u16 width, u16 height) {
    if (backend) {
        backend->resized(backend, width, height);
    } else {
        KWARN("renderer backend does not exist to accept resize: %i %i", width, height);
    }
}

b8 renderer_draw_frame(render_packet* packet) {
    // If the begin frame returned successfully, mid-frame operations may continue.
    if (renderer_begin_frame(packet->delta_time)) {
        mat4 projection = mat4::perspective(deg_to_rad(45.0f), 1280 / 720.0f, 0.1f, 1000.0f);
        static f32 z = -1.0f;
        z -= 0.005f;
        mat4 view = mat4::translation((vec3){0, 0, z});

        backend->update_global_state(projection, view, vec3::zero(), vec4::one(), 0);

        // End the frame. If this fails, it is likely unrecoverable.
        b8 result = renderer_end_frame(packet->delta_time);

        if (!result) {
            KERROR("renderer_end_frame failed. Application shutting down...");
            return FALSE;
        }
    }

    return TRUE;
}