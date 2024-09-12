#pragma once

/** \file material_bundle.h */

#include "framework64/allocator.h"
#include "framework64/data_io.h"

typedef struct fw64MaterialBundle fw64MaterialBundle;

typedef struct fw64AssetDatabase fw64AssetDatabase;

#ifdef __cplusplus
extern "C" {
#endif

fw64MaterialBundle* fw64_material_bundle_load_from_datasource(fw64DataSource* data_source, fw64AssetDatabase* assets, fw64Allocator* allocator);
void fw64_material_bundle_delete(fw64MaterialBundle* material_bundle, fw64Allocator* allocator);

#ifdef __cplusplus
}
#endif
