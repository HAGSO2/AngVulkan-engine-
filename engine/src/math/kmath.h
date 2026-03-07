#pragma once

#include "defines.h"

#define K_PI 3.14159265358979323846f
#define K_PI_2 2.0f * K_PI
#define K_HALF_PI 0.5f * K_PI
#define K_QUARTER_PI 0.25f * K_PI
#define K_ONE_OVER_PI 1.0f / K_PI
#define K_ONE_OVER_TWO_PI 1.0f / K_PI_2
#define K_SQRT_TWO 1.41421356237309504880f
#define K_SQRT_THREE 1.73205080756887729352f
#define K_SQRT_ONE_OVER_TWO 0.70710678118654752440f
#define K_SQRT_ONE_OVER_THREE 0.57735026918962576450f
#define K_DEG2RAD_MULTIPLIER K_PI / 180.0f
#define K_RAD2DEG_MULTIPLIER 180.0f / K_PI

// The multiplier to convert seconds to milliseconds.
#define K_SEC_TO_MS_MULTIPLIER 1000.0f

// The multiplier to convert milliseconds to seconds.
#define K_MS_TO_SEC_MULTIPLIER 0.001f

// A huge number that should be larger than any valid number used.
#define K_INFINITY 1e30f

// Smallest positive number where 1.0 + FLOAT_EPSILON != 0
#define K_FLOAT_EPSILON 1.192092896e-07f

// ------------------------------------------
// General math functions
// ------------------------------------------
KAPI f32 ksin(f32 x);
KAPI f32 kcos(f32 x);
KAPI f32 ktan(f32 x);
KAPI f32 kacos(f32 x);
KAPI f32 ksqrt(f32 x);
KAPI f32 kabs(f32 x);

/**
 * Indicates if the value is a power of 2. 0 is considered _not_ a power of 2.
 * @param value The value to be interpreted.
 * @returns True if a power of 2, otherwise false.
 */
KINLINE b8 is_power_of_2(u64 value) {
    return (value != 0) && ((value & (value - 1)) == 0);
}

KAPI i32 krandom();
KAPI i32 krandom_in_range(i32 min, i32 max);

KAPI f32 fkrandom();
KAPI f32 fkrandom_in_range(f32 min, f32 max);