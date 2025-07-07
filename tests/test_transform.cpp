#include "gtest/gtest.h"

#include <framework64/matrix.h>
#include <framework64/transform.h>
#include <framework64/types.h>

TEST(fw64Transform, init) {
    fw64Transform xform;
    fw64_transform_init(&xform);

    ASSERT_EQ(xform.parent, nullptr);
    ASSERT_EQ(xform.next_sibling, nullptr);
    ASSERT_EQ(xform.first_child, nullptr);
}

TEST(fw64Transform, add_first_child) {
    fw64Transform parent;
    fw64_transform_init(&parent);

    fw64Transform child;
    fw64_transform_init(&child);

    fw64_transform_add_child(&parent, &child);

    ASSERT_EQ(parent.first_child, &child);
    ASSERT_EQ(child.parent, &parent);
    ASSERT_EQ(child.next_sibling, nullptr);
}

TEST(fw64Transform, add_more_children) {
    fw64Transform parent;
    fw64_transform_init(&parent);

    fw64Transform child1, child2;
    fw64_transform_init(&child1);
    fw64_transform_init(&child2);

    fw64_transform_add_child(&parent, &child1);
    fw64_transform_add_child(&parent, &child2);

    ASSERT_EQ(parent.first_child, &child2);
    ASSERT_EQ(child2.parent, &parent);
    ASSERT_EQ(child2.next_sibling, &child1);

    ASSERT_EQ(child1.parent, &parent);
    ASSERT_EQ(child1.next_sibling, nullptr);
}

TEST(fw64Transform, remove_only_child) {
    fw64Transform parent;
    fw64_transform_init(&parent);

    fw64Transform child;
    fw64_transform_init(&child);

    fw64_transform_add_child(&parent, &child);

    int result = fw64_transform_remove_child(&parent, &child);
    ASSERT_NE(result, 0);

    ASSERT_EQ(child.parent, nullptr);
    ASSERT_EQ(parent.first_child, nullptr);
}

TEST(fw64Transform, remove_first_child) {
    fw64Transform parent;
    fw64_transform_init(&parent);

    fw64Transform child1, child2, child3;
    fw64_transform_init(&child1);
    fw64_transform_init(&child2);
    fw64_transform_init(&child3);

    fw64_transform_add_child(&parent, &child3);
    fw64_transform_add_child(&parent, &child2);
    fw64_transform_add_child(&parent, &child1);

    int result = fw64_transform_remove_child(&parent, &child1);
    ASSERT_NE(result, 0);

    ASSERT_EQ(child1.parent, nullptr);
    ASSERT_EQ(child1.next_sibling, nullptr);
    ASSERT_EQ(child2.parent, &parent);
    ASSERT_EQ(child3.parent, &parent);
    ASSERT_EQ(parent.first_child, &child2);
    ASSERT_EQ(child2.next_sibling, &child3);
}

TEST(fw64Transform, remove_middle_child) {
    fw64Transform parent;
    fw64_transform_init(&parent);

    fw64Transform child1, child2, child3;
    fw64_transform_init(&child1);
    fw64_transform_init(&child2);
    fw64_transform_init(&child3);

    fw64_transform_add_child(&parent, &child3);
    fw64_transform_add_child(&parent, &child2);
    fw64_transform_add_child(&parent, &child1);

    int result = fw64_transform_remove_child(&parent, &child2);
    ASSERT_NE(result, 0);

    ASSERT_EQ(child2.parent, nullptr);
    ASSERT_EQ(child2.next_sibling, nullptr);
    ASSERT_EQ(child1.parent, &parent);
    ASSERT_EQ(child3.parent, &parent);
    ASSERT_EQ(parent.first_child, &child1);
    ASSERT_EQ(child1.next_sibling, &child3);
}

TEST(fw64Transform, remove_last_child) {
    fw64Transform parent;
    fw64_transform_init(&parent);

    fw64Transform child1, child2, child3;
    fw64_transform_init(&child1);
    fw64_transform_init(&child2);
    fw64_transform_init(&child3);

    fw64_transform_add_child(&parent, &child3);
    fw64_transform_add_child(&parent, &child2);
    fw64_transform_add_child(&parent, &child1);

    int result = fw64_transform_remove_child(&parent, &child3);
    ASSERT_NE(result, 0);
    
    ASSERT_EQ(child1.parent, &parent);
    ASSERT_EQ(child2.parent, &parent);
    ASSERT_EQ(child3.parent, nullptr);
    ASSERT_EQ(child3.next_sibling, nullptr);
    ASSERT_EQ(parent.first_child, &child1);
    ASSERT_EQ(child1.next_sibling, &child2);
    ASSERT_EQ(child2.next_sibling, nullptr);
}

TEST(fw64Transform, add_child_with_existing_parent) {
    fw64Transform parent1, parent2;
    fw64_transform_init(&parent1);
    fw64_transform_init(&parent2);

    fw64Transform child;
    fw64_transform_init(&child);

    fw64_transform_add_child(&parent1, &child);

    ASSERT_EQ(parent1.first_child, &child);
    ASSERT_EQ(child.parent, &parent1);

    fw64_transform_add_child(&parent2, &child);

    ASSERT_EQ(parent1.first_child, nullptr);
    ASSERT_EQ(parent2.first_child, &child);
    ASSERT_EQ(child.parent, &parent2);
}

TEST(fw64Transform, update_child_matrix) {
    fw64Transform parent, child;
    fw64_transform_init(&parent);
    fw64_transform_init(&child);

    vec3_set(&parent.position, 10.0f, 0.0f, 0.0f);
    vec3_set_all(&parent.scale, 2.0f);

    vec3_set(&child.position, 0.0f, 0.0f, 25.0f);

    fw64_transform_add_child(&parent, &child);
    fw64_transform_update_matrix(&parent);

    float expectedParentMatrix[16], child_local_matrix[16], expected_child_matrix[16];
    matrix_from_trs(expectedParentMatrix, &parent.position, &parent.rotation, &parent.scale);
    matrix_from_trs(child_local_matrix, &child.position, &child.rotation, &child.scale);
    matrix_multiply(expectedParentMatrix, child_local_matrix, expected_child_matrix);

    ASSERT_TRUE(matrix_equals(expectedParentMatrix, parent.world_matrix, EPSILON));
    ASSERT_TRUE(matrix_equals(expected_child_matrix, child.world_matrix, EPSILON));
}