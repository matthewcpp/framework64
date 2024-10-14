#pragma once

#include "framework64/render_pass.h"

#include "framework64/render_queue.h"

struct fw64RenderPass{
    fw64Allocator* allocator;
    fw64RenderQueue render_queue;
};
