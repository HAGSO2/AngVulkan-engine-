#include "vulkan_backend.h"

#include "vulkan_types.inl"
#include "vulkan_platform.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"

#include "core/logger.h"
#include "utils/Astring.h"

#include "platform/platform.h"

// static Vulkan context
static vulkan_context context{};

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);

i32 find_memory_index(u32 type_filter, u32 property_flags);

b8 vulkan_renderer_backend_initialize(renderer_backend* backend, const char* application_name, const char* engine_name, struct platform_state* plat_state
, vulkan_options* options) {

    // Function pointers
    context.find_memory_index = find_memory_index;

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

    // Validation layers.
    vector<const char*> required_validation_layer_names = vector<const char*>();
    u32 required_validation_layer_count = 0;

// If validation should be done, get a list of the required validation layert names
// and make sure they exist. Validation layers should only be enabled on non-release builds.
#if defined(_DEBUG)
    KINFO("Validation layers enabled. Enumerating...");

    // The list of validation layers required.
    required_validation_layer_names = vector<const char*>();
    required_validation_layer_names.push_back("VK_LAYER_KHRONOS_validation");
    required_validation_layer_count = required_validation_layer_names.size();

    // Obtain a list of available validation layers
    u32 available_layer_count = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, 0));
    vector<VkLayerProperties> available_layers = vector<VkLayerProperties>(available_layer_count);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data()));

    // Verify all required layers are available.
    for (u32 i = 0; i < required_validation_layer_count; ++i) {
        KINFO("Searching for layer: %s...", required_validation_layer_names[i]);
        b8 found = FALSE;
        for (u32 j = 0; j < available_layer_count; ++j) {
            if (strings_equal(required_validation_layer_names[i], available_layers[j].layerName)) {
                found = TRUE;
                KINFO("Found.");
                break;
            }
        }

        if (!found) {
            KFATAL("Required validation layer is missing: %s", required_validation_layer_names[i]);
            return FALSE;
        }
    }
    KINFO("All required validation layers are present.");
#endif

    create_info.enabledLayerCount = required_validation_layer_count;
    create_info.ppEnabledLayerNames = required_validation_layer_names.data();

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

    // Swapchain
    vulkan_swapchain_create(
        &context,
        context.framebuffer_width,
        context.framebuffer_height,
        &context.swapchain);


    KINFO("Vulkan renderer initialized successfully.");
    return TRUE;
}

void vulkan_renderer_backend_shutdown(renderer_backend* backend) {
    //Destroying in opposite order of creation

    // Swapchain
    vulkan_swapchain_destroy(&context, &context.swapchain);

    //Device
    KINFO("Destroying Vulkan device...");
    vulkan_device_destroy(&context);

    //Surface
    KINFO("Destroying Vulkan surface...");
    if (context.surface) {
        vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
        context.surface = 0;
    }

    KDEBUG("Destroying Vulkan debugger...");
    if (context.debug_messenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
        func(context.instance, context.debug_messenger, context.allocator);
    }

    KDEBUG("Destroying Vulkan instance...");
    vkDestroyInstance(context.instance, context.allocator);
}

void vulkan_renderer_backend_on_resized(renderer_backend* backend, u16 width, u16 height) {
}

b8 vulkan_renderer_backend_begin_frame(renderer_backend* backend, f32 delta_time) {
    return TRUE;
}

b8 vulkan_renderer_backend_end_frame(renderer_backend* backend, f32 delta_time) {
    return TRUE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {
    switch (message_severity) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            KERROR(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            KWARN(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            KINFO(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            KTRACE(callback_data->pMessage);
            break;
    }
    return VK_FALSE;
}

i32 find_memory_index(u32 type_filter, u32 property_flags) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(context.device.physical_device, &memory_properties);

    for (u32 i = 0; i < memory_properties.memoryTypeCount; ++i) {
        // Check each memory type to see if its bit is set to 1.
        if (type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags) {
            return i;
        }
    }

    KWARN("Unable to find suitable memory type!");
    return -1;
}