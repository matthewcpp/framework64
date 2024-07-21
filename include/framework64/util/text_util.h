#pragma once

/** \file sparse_set.h */

#include "framework64/font.h"

/** Computes the position for the supplied string to be centered in a rect of the givens ize */
IVec2 fw64_text_util_center_string(fw64Font* font, const char* str, const IVec2* size);
