#include "framework64/util/renderpass_util.h"

#include "framework64/matrix.h"

void fw64_renderpass_util_ortho2d(fw64RenderPass* renderpass) {
    const fw64Viewport* viewport = fw64_renderpass_get_viewport(renderpass);
    Vec2 viewport_size = {(float)viewport->size.x, (float)viewport->size.y};

    float view[16], projection[16];
    matrix_set_identity(view);
    matrix_ortho2d(projection, 0, viewport_size.x, viewport_size.y, 0);

    fw64_renderpass_set_view_matrix(renderpass, view);
    fw64_renderpass_set_projection_matrix(renderpass, projection, NULL);

    fw64_renderpass_set_depth_testing_enabled(renderpass, 0);
}
