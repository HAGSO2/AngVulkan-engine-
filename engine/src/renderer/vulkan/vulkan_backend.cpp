#include "vulkan_backend.h"

#include "vulkan_types.inl"
#include "vulkan_platform.h"
#include "vulkan_device.h"

#include "core/logger.h"

#include "platform/platform.h"

// static Vulkan context
static vulkan_context context{};

b8 vulkan_renderer_backend_initialize(renderer_backend* backend, const char* application_name, const char* engine_name, struct platform_state* plat_state
, vulkan_options* options) {

    // Setup Vulkan instance.
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = VK_API_VERSION_1_2;
    app_info.pApplicationName = application_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = engine_name;
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    vector<const char*> required_extensions = vector<const char*>();
    required_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    platform_get_required_extension_names(required_extensions);
    #if defined(_DEBUG)
    required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);  // debug utilities

    KDEBUG("Required extensions:");
    for (u32 i = 0; i < required_extensions.size(); ++i) {
        KDEBUG(required_extensions[i]);
    }
#endif

    create_info.enabledExtensionCount = required_extensions.size();
    create_info.ppEnabledExtensionNames = required_extensions.data();

    //TODO: Podría poner más layers de debug

    VK_CHECK(vkCreateInstance(&create_info, context.allocator, &context.instance));
    KINFO("Vulkan Instance created.");

    // Surface
    KDEBUG("Creating Vulkan surface...");
    if (!platform_create_vulkan_surface(plat_state, &context)) {
        KERROR("Failed to create platform surface!");
        return FALSE;
    }
    KDEBUG("Vulkan surface created.");

    // Device creation
    if (!vulkan_device_create(&context, options)) {
        KERROR("Failed to create device!");
        return FALSE;
    }

    KINFO("Vulkan renderer initialized successfully.");
    return TRUE;
}