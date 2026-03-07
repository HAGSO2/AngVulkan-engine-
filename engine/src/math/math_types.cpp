#include "math_types.h"
#include "kmath.h"

// ------------------------------------------
// Vector 2
// ------------------------------------------

KINLINE f32 vec2::length_squared(){return x*x+y*y;}
KINLINE f32 vec2::lenght(){return ksqrt(length_squared());}

KINLINE void vec2::normalize() {
    f32 lenght = ksqrt(length_squared());
    x /= lenght;
    y /= lenght;
}

KINLINE const b8 vec2::compare(vec2 b, f32 tolerance) {
    if(kabs(x - b.x) > tolerance)
        return FALSE;
    if(kabs(y - b.y) > tolerance)
        return FALSE;
    return TRUE;
}

KINLINE f32 vec2::distance(vec2 b) {
    return subs(*this,b).lenght();
}

// ------------------------------------------
// Vector 3
// ------------------------------------------

KINLINE f32 vec3::length_squared(){return x*x+y*y+z*z;}
KINLINE f32 vec3::lenght(){return ksqrt(length_squared());}

KINLINE void vec3::normalize() {
    f32 lenght = ksqrt(length_squared());
    x /= lenght;
    y /= lenght;
    z /= lenght;
}

KINLINE f32 vec3::dot(vec3 b){
    f32 p = 0;
    p += x * b.x;
    p += y * b.y;
    p += z * b.z;
    return p;
}
KINLINE vec3 vec3::cross(vec3 a, vec3 b){
    return vec3{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

KINLINE const b8 vec3::compare(vec3 b, f32 tolerance) {
    if(kabs(x - b.x) > tolerance)
        return FALSE;
    if(kabs(y - b.y) > tolerance)
        return FALSE;
    if (kabs(z - b.z) > tolerance)
        return FALSE;
    return TRUE;
}

KINLINE f32 vec3::distance(vec3 b) {
    return subs(*this,b).lenght();
}