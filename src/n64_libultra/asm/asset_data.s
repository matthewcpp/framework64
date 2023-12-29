.include "macros.inc"

.section .data

.balign 16
glabel _fw64bundleSegmentRomStart
.incbin "${game_asset_dir}/assets.dat"
glabel _fw64bundleSegmentRomEnd
