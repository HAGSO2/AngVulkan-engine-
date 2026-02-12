#pragma once
#include "vulkan_types.inl"

void vulkan_command_buffer_allocate(
    vulkan_context* context,
    VkCommandPool pool,
    b8 is_primary,
    vulkan_command_buffer* out_command_buffer);

void vulkan_command_buffer_free(
    vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* command_buffer);