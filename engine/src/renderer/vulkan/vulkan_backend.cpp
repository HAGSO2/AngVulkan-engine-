#include "vulkan_backend.h"

#include "vulkan_types.inl"
#include "vulkan_platform.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_renderpass.h"
#include "vulkan_framebuffer.h"
#include "vulkan_utils.h"
#include "vulkan_command_buffer.h"

#include "core/logger.h"
#include "utils/Astring.h"

#include "platform/platform.h"

// static Vulkan context
static vulkan_context context{};
static u32 cached_framebuffer_width = 0;
static u32 cached_framebuffer_height = 0;

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);

i32 find_memory_index(u32 type_filter, u32 property_flags);
void create_command_buffers(renderer_backend* backend);
void regenerate_framebuffers(renderer_backend* backend, vulkan_swapchain* swapchain, vulkan_renderpass* renderpass);
b8 recreate_swapchain(renderer_backend* backend);

b8 vulkan_renderer_backend_initialize(renderer_backend* backend, const char* application_name, const char* engine_name, struct platform_state* plat_state
, Application* app) {

    // Function pointers
    context.find_memory_index = find_memory_index;

    app->application_get_framebuffer_size(&cached_framebuffer_width, &cached_framebuffer_height);
    context.framebuffer_width = (cached_framebuffer_width != 0) ? cached_framebuffer_width : 800;
    context.framebuffer_height = (cached_framebuffer_height != 0) ? cached_framebuffer_height : 600;
    cached_framebuffer_width = 0;
    cached_framebuffer_height = 0;

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
    if (!vulkan_device_create(&context,app->GetVlkOptions())) {
        KERROR("Failed to create device!");
        return FALSE;
    }

    // Swapchain
    vulkan_swapchain_create(
        &context,
        context.framebuffer_width,
        context.framebuffer_height,
        &context.swapchain);
    
    //Renderpass

    VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = context.surfaceFormat.format;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentDescription attachments[] = {
            colorAttachment};

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0; // This is an index into the attachments array
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDesc = {};
        subpassDesc.colorAttachmentCount = 1;
        subpassDesc.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo rpInfo = {};
        rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rpInfo.pAttachments = attachments;
        rpInfo.attachmentCount = ARRAYSIZE(attachments);
        rpInfo.subpassCount = 1;
        rpInfo.pSubpasses = &subpassDesc;

    vulkan_renderpass_create(
        &context,
        &context.main_renderpass,
        0, 0, context.framebuffer_width, context.framebuffer_height,
        0.0f, 0.0f, 0.2f, 1.0f,
        1.0f,
        0);

    VK_CHECK(vkCreateRenderPass(context.device.logical_device,&rpInfo, context.allocator, &context.main_renderpass.handle));

    // VkAttachmentDescription attachment = {};
    // //NOTE: This can be changed so load frame is different.
    // attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    // attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // //NOTE: Multisampling settings here
    // attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    // attachment.format = context.surfaceFormat.format;

    // VkAttachmentReference colorAttachmentRef = {};
    // colorAttachmentRef.attachment = 0; // This is an index into the attachments array
    // colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // VkSubpassDescription subpassDesc = {};
    // subpassDesc.colorAttachmentCount = 1;
    // subpassDesc.pColorAttachments = &colorAttachmentRef;

    // VkAttachmentDescription attachments[] = {
    //     attachment
    // };

    // VkRenderPassCreateInfo rpInfo = {};
    // rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    // rpInfo.pAttachments = &attachment;
    // rpInfo.attachmentCount = 1;
    // //rpInfo.pAttachments = attachments;
    // //rpInfo.attachmentCount = ARRAYSIZE(attachments);
    // rpInfo.subpassCount = 1;
    // rpInfo.pSubpasses = &subpassDesc;
    // VK_CHECK(vkCreateRenderPass(context.device.logical_device,&rpInfo, context.allocator, &context.main_renderpass.handle));
    
    // Swapchain framebuffers.
    
    // context.swapchain.framebuffers = vector<vulkan_framebuffer>(context.swapchain.image_count);
    // regenerate_framebuffers(backend, &context.swapchain, &context.main_renderpass);
    VkFramebufferCreateInfo fbInfo = {};
    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbInfo.renderPass = context.main_renderpass.handle;
    fbInfo.width = context.framebuffer_width;
    fbInfo.height = context.framebuffer_height;
    fbInfo.layers = 1;
    fbInfo.attachmentCount = 1;

    context.swapchain.framebuffers = vector<vulkan_framebuffer>(context.swapchain.image_count);

    for (uint32_t i = 0; i < context.swapchain.image_count; i++)
    {
        fbInfo.pAttachments = &context.swapchain.views[i];
        VK_CHECK(vkCreateFramebuffer(context.device.logical_device, &fbInfo, 0, &context.swapchain.framebuffers[i].handle));
    }
    
    //Command Pool
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = context.device.graphics_queue_index;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK(vkCreateCommandPool(context.device.logical_device,&poolInfo,context.allocator,&context.device.graphics_command_pool));

    //Command Buffer

    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandBufferCount = 1;
    info.commandPool = context.device.graphics_command_pool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    VK_CHECK(vkAllocateCommandBuffers(context.device.logical_device, &info, &context.cmd));

    //Sync objects
    VkSemaphoreCreateInfo sema_info = {};
    sema_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VK_CHECK(vkCreateSemaphore(context.device.logical_device, &sema_info, context.allocator, &context.submit_semaphore));
    VK_CHECK(vkCreateSemaphore(context.device.logical_device, &sema_info, context.allocator, &context.aquire_semaphore));

    VkFenceCreateInfo fence_info = {};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    VK_CHECK(vkCreateFence(context.device.logical_device, &fence_info, 0, &context.imgAvailableFence));

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
    // Update the "framebuffer size generation", a counter which indicates when the
    // framebuffer size has been updated.
    cached_framebuffer_width = width;
    cached_framebuffer_height = height;
    context.framebuffer_size_generation++;

    KINFO("Vulkan renderer backend->resized: w/h/gen: %i/%i/%llu", width, height, context.framebuffer_size_generation);
}

b8 vulkan_renderer_backend_begin_frame(renderer_backend* backend, f32 delta_time) {

    // // Check if the framebuffer has been resized. If so, a new swapchain must be created.
    // if (context.framebuffer_size_generation != context.framebuffer_size_last_generation) {
    //     VkResult result = vkDeviceWaitIdle(context.device.logical_device);
    //     if (!vulkan_result_is_success(result)) {
    //         KERROR("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (2) failed: '%s'", vulkan_result_string(result, TRUE));
    //         return FALSE;
    //     }

    //     // If the swapchain recreation failed (because, for example, the window was minimized),
    //     // boot out before unsetting the flag.
    //     if (!recreate_swapchain(backend)) {
    //         return FALSE;
    //     }

    //     KINFO("Resized, booting.");
    //     return FALSE;
    // }

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

    VkCommandBuffer cmd = context.cmd;
    vkResetCommandBuffer(cmd,0);

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK(vkBeginCommandBuffer(cmd, &begin_info));

    VkClearValue clearValue = {};
    clearValue.color = {0,0,0.5,0.5};

    VkRenderPassBeginInfo rpBeginInfo = {};
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderArea.offset.x = context.main_renderpass.x;
    rpBeginInfo.renderArea.offset.y = context.main_renderpass.y;
    rpBeginInfo.renderArea.extent.width = context.main_renderpass.w;
    rpBeginInfo.renderArea.extent.height = context.main_renderpass.h;
    // KDEBUG("Dimensions: x:%d y:%d, h:%d, w:%d",
    //     rpBeginInfo.renderArea.offset.x,
    //     rpBeginInfo.renderArea.offset.y,
    //     rpBeginInfo.renderArea.extent.height,
    //     rpBeginInfo.renderArea.extent.width);
    rpBeginInfo.clearValueCount = 1;
    rpBeginInfo.pClearValues = &clearValue;
    rpBeginInfo.renderPass = context.main_renderpass.handle;
    rpBeginInfo.framebuffer = context.swapchain.framebuffers[context.image_index].handle;
    vkCmdBeginRenderPass(cmd, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // // Rendering Commands
    // {
    //     VkViewport viewport = {};
    //     viewport.maxDepth = 1.0f;
    //     viewport.width = context.main_renderpass.w;
    //     viewport.height = context.main_renderpass.h;

    //     VkRect2D scissor = {};
    //     scissor.offset.x = context.main_renderpass.x;
    //     scissor.offset.y = context.main_renderpass.y;
    //     scissor.extent.width = context.main_renderpass.w;
    //     scissor.extent.height = context.main_renderpass.h;

    //     vkCmdSetViewport(cmd, 0, 1, &viewport);
    //     vkCmdSetScissor(cmd, 0, 1, &scissor);

    //     vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vkcontext->pipeLayout,
    //                             0, 1, &vkcontext->descSet, 0, 0);

    //     vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vkcontext->pipeline);
    //     vkCmdDraw(cmd, 6, 1, 0, 0);
    //     // Actually draw
    //     // Bind pipe
    //     // Bind desc
    //     // Draw
    //     // Reapeat
    // }

    vkCmdEndRenderPass(cmd);
    
    VK_CHECK(vkEndCommandBuffer(cmd)); //<--Fallo It is invalid to issue this call inside an active VkRenderPass 0xe000000000e

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    // Submit the queue
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd;
    submit_info.pWaitDstStageMask = &waitStage;
    submit_info.pSignalSemaphores = &context.submit_semaphore;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &context.aquire_semaphore;
    submit_info.waitSemaphoreCount = 1;

    VK_CHECK(vkQueueSubmit(context.device.graphics_queue, 1, &submit_info, 0));

    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &context.swapchain.handle;
    present_info.pImageIndices = &context.image_index;
    present_info.pWaitSemaphores = &context.submit_semaphore;
    present_info.waitSemaphoreCount = 1;
    VK_CHECK(vkQueuePresentKHR(context.device.present_queue, &present_info)); 
    //WARN: Aquí a puesto graphics queue
    //WARN: Al hacer un rezise esta parte falla. 
    //NOTE: Quizá se deba a que tengo que actualizar y re-crear la cola con las dimensiones adecuadas
    VK_CHECK(vkDeviceWaitIdle(context.device.logical_device));
    //vkFreeCommandBuffers(context.device.logical_device, context.device.graphics_command_pool,1,&context.cmd);

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

void create_command_buffers(renderer_backend* backend) {
    if (context.graphics_command_buffers.empty()) {
        context.graphics_command_buffers = vector<vulkan_command_buffer>(context.swapchain.image_count);
    }

    context.graphics_command_buffers.clear();
    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        if (context.graphics_command_buffers[i].handle) {
            vulkan_command_buffer_free(
                &context,
                context.device.graphics_command_pool,
                &context.graphics_command_buffers[i]);
        }
        
        vulkan_command_buffer_allocate(
            &context,
            context.device.graphics_command_pool,
            TRUE,
            &context.graphics_command_buffers[i]);
    }

    KDEBUG("Vulkan command buffers created.");
}

void regenerate_framebuffers(renderer_backend* backend, vulkan_swapchain* swapchain, vulkan_renderpass* renderpass) {
    for (u32 i = 0; i < swapchain->image_count; ++i) {
        // TODO: make this dynamic based on the currently configured attachments
        u32 attachment_count = 2;
        vector<VkImageView> attachments = {
            swapchain->views[i],
            swapchain->depth_attachment.view};

        vulkan_framebuffer_create(
            &context,
            renderpass,
            context.framebuffer_width,
            context.framebuffer_height,
            attachment_count,
            attachments,
            &context.swapchain.framebuffers[i]);
    }
}

b8 recreate_swapchain(renderer_backend* backend) {
    // If already being recreated, do not try again.
    if (context.recreating_swapchain) {
        KDEBUG("recreate_swapchain called when already recreating. Booting.");
        return FALSE;
    }

    // Detect if the window is too small to be drawn to
    if (context.framebuffer_width == 0 || context.framebuffer_height == 0) {
        KDEBUG("recreate_swapchain called when window is < 1 in a dimension. Booting.");
        return FALSE;
    }

    // Mark as recreating if the dimensions are valid.
    context.recreating_swapchain = TRUE;

    // Wait for any operations to complete.
    vkDeviceWaitIdle(context.device.logical_device);

    //TODO:
    // // Clear these out just in case.
    // for (u32 i = 0; i < context.swapchain.image_count; ++i) {
    //     context.images_in_flight[i] = 0;
    // }

    // Requery support
    vulkan_device_query_swapchain_support(
        context.device.physical_device,
        context.surface,
        &context.device.swapchain_support);
    vulkan_device_detect_depth_format(&context.device);

    vulkan_swapchain_recreate(
        &context,
        cached_framebuffer_width,
        cached_framebuffer_height,
        &context.swapchain);

    // Sync the framebuffer size with the cached sizes.
    context.framebuffer_width = cached_framebuffer_width;
    context.framebuffer_height = cached_framebuffer_height;
    context.main_renderpass.w = context.framebuffer_width;
    context.main_renderpass.h = context.framebuffer_height;
    cached_framebuffer_width = 0;
    cached_framebuffer_height = 0;

    // Update framebuffer size generation.
    context.framebuffer_size_last_generation = context.framebuffer_size_generation;

    // cleanup swapchain
    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        vulkan_command_buffer_free(&context, context.device.graphics_command_pool, &context.graphics_command_buffers[i]);
    }

    // Framebuffers.
    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        vulkan_framebuffer_destroy(&context, &context.swapchain.framebuffers[i]);
    }

    context.main_renderpass.x = 0;
    context.main_renderpass.y = 0;
    context.main_renderpass.w = context.framebuffer_width;
    context.main_renderpass.h = context.framebuffer_height;

    regenerate_framebuffers(backend, &context.swapchain, &context.main_renderpass);

    create_command_buffers(backend);

    // Clear the recreating flag.
    context.recreating_swapchain = FALSE;

    return TRUE;
}