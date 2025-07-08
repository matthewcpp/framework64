#pragma once

#include "framework64/engine.h"

int fw64_n64_libultra_engine_init(fw64Engine* engine, int asset_count);
void fw64_n64_libultra_engine_update(fw64Engine* engine);

void fw64_n64_libultra_engine_finalize_frame(fw64Engine* engine);