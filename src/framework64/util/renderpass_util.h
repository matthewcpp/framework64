#pragma once

/** \file renderpass_util.h */

#include "framework64/render_pass.h"

/** Sets an ortho2d camera projection using the current viewport of the renderpass. */
void fw64_renderpass_util_ortho2d(fw64RenderPass* renderpass);
