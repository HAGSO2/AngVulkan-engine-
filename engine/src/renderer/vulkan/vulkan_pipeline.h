#pragma once
#include <vulkan/vulkan.h>
#include "core/logger.h"
#include "core/application.h"
#include "vulkan_types.inl"

void vulkan_pipeline_create(vulkan_context* c);

void set_descriptors(vulkan_context* c);

void vulkan_pipeline_draw(vulkan_context* context);

void vulkan_pipeline_destroy(vulkan_context* c);