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

    //FrameBuffer
    vkCreateFramebuffer(context.device.logical_device, &fb_info, context.allocator, &context.framebuffer)

    // Main subpass
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // Attachments TODO: make this configurable.
    u32 attachment_description_count = 2;
    VkAttachmentDescription* attachment_descriptions = new VkAttachmentDescription[attachment_description_count];

    // Color attachment
    VkAttachmentDescription color_attachment;
    color_attachment.format = context.swapchain.image_format.format;  // TODO: configurable
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;      // Do not expect any particular layout before render pass starts.
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;  // Transitioned to after the render pass
    color_attachment.flags = 0;

    attachment_descriptions[0] = color_attachment;

    VkAttachmentReference color_attachment_reference;
    color_attachment_reference.attachment = 0;  // Attachment description array index
    color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_reference;

    // Depth attachment, if there is one
    VkAttachmentDescription depth_attachment = {};
    depth_attachment.format = context.device.depth_format;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    attachment_descriptions[1] = depth_attachment;
    
    VkRenderPassCreateInfo rp_info = {};
    rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp_info.attachmentCount = attachment_description_count;
    rp_info.pAttachments = attachment_descriptions;
    rp_info.subpassCount = 1;
    rp_info.pSubpasses = &subpass;
    VK_CHECK(vkCreateRenderPass(context.device.logical_device, &rp_info, 0, &context.renderpass));

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = context.device.graphics_queue_index;
    VK_CHECK(vkCreateCommandPool(context.device.logical_device,&poolInfo,context.allocator,&context.device.graphics_command_pool));

    //Sync objects
    VkSemaphoreCreateInfo sema_info = {};
    sema_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VK_CHECK(vkCreateSemaphore(context.device.logical_device, &sema_info, context.allocator, &context.submit_semaphore));
    VK_CHECK(vkCreateSemaphore(context.device.logical_device, &sema_info, context.allocator, &context.aquire_semaphore));

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

    // Acquire the next image from the swap chain. Pass along the semaphore that should signaled when this completes.
    // This same semaphore will later be waited on by the queue submission to ensure this image is available.
    if (!vulkan_swapchain_acquire_next_image_index(
            &context,
            &context.swapchain,
            UINT64_MAX,
            context.aquire_semaphore,
            0,
            &context.image_index)) {
        return FALSE;
    }

    //Rendering commands
    VkCommandBuffer cmd;
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandBufferCount = 1;
    allocInfo.commandPool = context.device.graphics_command_pool;
    VK_CHECK(vkAllocateCommandBuffers(context.device.logical_device, &allocInfo ,&cmd));

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK(vkBeginCommandBuffer(cmd, &begin_info));

    VkRenderPassBeginInfo rpBegin_info = {};
    rpBegin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBegin_info.renderPass = context.renderpass;
    //WARN: Esto hay que definirlo
    //rpBegin_info.renderArea.extent = context.screensize;

    vkCmdBeginRenderPass(cmd,&rpBegin_info,VK_SUBPASS_CONTENTS_INLINE);

    //Render commands
    {
        VkClearColorValue color = {0,0,0.5,0.5};
        VkImageSubresourceRange range ={};
        range.layerCount = 1;
        range.levelCount = 1;
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vkCmdClearColorImage(cmd, context.swapchain.images[context.image_index], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, &color, 1, &range);
    }
    
    VK_CHECK(vkEndCommandBuffer(cmd));

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    // Submit the queue
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pWaitDstStageMask = &waitStage;
    submit_info.commandBufferCount = 1;
    //submit_info.pCommandBuffers = &command_buffer->handle;
    submit_info.pCommandBuffers = &cmd;
    submit_info.pSignalSemaphores = &context.submit_semaphore;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &context.aquire_semaphore;
    submit_info.waitSemaphoreCount = 1;

    VK_CHECK(vkQueueSubmit(context.device.graphics_queue, 1, &submit_info, 0));

    // Wait for it to finish
    //VK_CHECK(vkQueueWaitIdle(queue));

    // if (!vulkan_swapchain_acquire_next_image_index(
    //         &context,
    //         &context.swapchain,
    //         UINT64_MAX,
    //         context.image_available_semaphores[context.current_frame],
    //         0,
    //         &context.image_index)) {
    //     return FALSE;
    // }

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &context.swapchain.handle;
    present_info.pImageIndices = &context.image_index;
    present_info.pWaitSemaphores = &context.submit_semaphore;
    present_info.waitSemaphoreCount = 1;
    VK_CHECK(vkQueuePresentKHR(context.device.present_queue, &present_info));
    vkDeviceWaitIdle(context.device.logical_device);
    vkFreeCommandBuffers(context.device.logical_device, context.device.graphics_command_pool,1,&cmd);

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