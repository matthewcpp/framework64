#pragma once

/** \file query.h */

#include "framework64/node.h"
#include "framework64/vec3.h"

#ifndef Fw64_COLLISION_QUERY_RESULT_SIZE
    #define Fw64_COLLISION_QUERY_RESULT_SIZE 5
#endif

typedef struct {
    fw64Node* node;
    Vec3 point;
    float distance;
} fw64RaycastHit;


typedef struct {
    fw64Node* node;
    Vec3 point;
} fw64OverlapSphereResult;

typedef struct {
    fw64OverlapSphereResult results[Fw64_COLLISION_QUERY_RESULT_SIZE];
    int count;
} fw64OverlapSphereQueryResult;

typedef struct {
    fw64Node* node;
    float tfirst;
    float tlast;
} IntersectMovingBoxResult;

typedef struct {
    IntersectMovingBoxResult results[Fw64_COLLISION_QUERY_RESULT_SIZE];
    int count;
} fw64IntersectMovingBoxQuery;

void fw64_intersect_moving_box_query_init(fw64IntersectMovingBoxQuery* query);
