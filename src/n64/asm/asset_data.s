.include "macros.inc"

.section .data

.balign 16
glabel _fw64bundleSegmentRomStart
.incbin "${FW64_N64_ASSET_DIR}/assets.dat"
glabel _fw64bundleSegmentRomEnd
