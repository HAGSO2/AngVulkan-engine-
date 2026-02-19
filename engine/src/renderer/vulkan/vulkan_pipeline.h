#pragma once
#include <vulkan/vulkan.h>
#include "core/logger.h"
#include "core/application.h"
#include "vulkan_types.inl"

void create_pipeline(vulkan_context* c);

void set_descriptors(vulkan_context* c);