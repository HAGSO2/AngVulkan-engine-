#pragma once

#include "defines.h"

/*
 * See for more on : 
 * OpenGl math library: https://github.com/g-truc/glm
 * Handmade math: https://github.com/HandmadeMath/HandmadeMath/tree/master
 * 
 */

struct vec2
{
    // An array of x, y
    union
    {
        struct { f32 x, y; };
        struct { f32 r, g; };
        struct { f32 s, t; };
        struct { f32 u, v; };
        
        f32 elements[2];
    };

    KSINLINE vec2 zero(){return {0,0};}
    KSINLINE vec2 one(){return vec2{1,1};}
    KSINLINE vec2 right(){return vec2{1,0};}
    KSINLINE vec2 up(){return vec2{0,1};}
    KINLINE f32 length_squared();
    KINLINE f32 lenght();
    KINLINE void normalize();
    KINLINE f32 distance(vec2 b);
    /**
     * @brief Compares all elements of vector_0 and vector_1 and ensures the difference
     * is less than tolerance.
     * 
     * @param tolerance The difference tolerance. Typically K_FLOAT_EPSILON or similar.
     * @return True if within tolerance; otherwise false. 
     */
    KINLINE const b8 compare(vec2 b, f32 tolerance);


    /***********************************************
     *                 OPERADORES                  *
     *                                             *
     *   SI VOY A USAR MUCHO UNA OPERACIÓN PUEDO   *
     * AÑADIR UNA FUNCIÓN KINLINE Y NO COMPLICARME *
     *                  LA VIDA.                   *
     ***********************************************/
     
    vec2 operator+(const vec2& b) const
    {
        return {x + b.x, y + b.y};
    }
    vec2& operator+=(const vec2& b)
    {
        x += b.x;
        y += b.y;
        return *this;
    }
    KSINLINE vec2 subs(vec2 a, vec2 b){
        return vec2{a.x-b.x,a.y-b.y};
    }
    vec2 operator-(const vec2& b) const
    {
        return {x - b.x, y - b.y};
    }
    vec2& operator-=(const vec2& b)
    {
        x -= b.x;
        y -= b.y;
        return *this;
    }
    vec2& operator*=(const f32 s)
    {
        x *= s;
        y *= s;
        return *this;
    }
    vec2& operator*=(const vec2 b){
        x *= b.x;
        y *= b.y;
        return *this;
    }
    vec2& operator/=(const f32 s)
    {
        x /= s;
        y /= s;
        return *this;
    }
    vec2& operator/=(const vec2& b){
        x /= b.x;
        y /= b.y;
        return *this;
    }
};

struct vec3
{
    // An array of x, y, z
    union
    {
        struct { f32 x, y, z; };
        struct { f32 r, g, b; };
        struct { f32 s, t, p; };
        struct { f32 u, v, w; };
        f32 elements[3];
    };
    KSINLINE vec3 zero(){return vec3{0,0,0};}
    KSINLINE vec3 one(){return vec3{1,1,1};}
    KSINLINE vec3 right(){return vec3{1,0,0};}
    KSINLINE vec3 up(){return vec3{0,1,0};}
    KSINLINE vec3 back(){return vec3{0,0,1};}
    KINLINE f32 length_squared();
    KINLINE f32 lenght();
    KINLINE void normalize();
    /**
     * @brief Returns the dot product between the provided vectors. Typically used
     * to calculate the difference in direction.
     */
    KINLINE f32 dot(vec3 b);
    /**
     * @brief Calculates and returns the cross product of the supplied vectors.
     * The cross product is a new vector which is orthoganal to both provided vectors.
    */
    KSINLINE vec3 cross(vec3 a, vec3 b);
    KINLINE const b8 compare(vec3 b, f32 tolerance);
    KINLINE f32 distance(vec3 b);


    /**************
     * OPERADORES *
     **************/
    vec3 operator+(const vec3& b) const
    {
        return {x + b.x, y + b.y, z + b.z};
    }
    vec3& operator+=(const vec3& b)
    {
        x += b.x;
        y += b.y;
        z += b.z;
        return *this;
    }
    KSINLINE vec3 subs(vec3 a, vec3 b){
        return vec3{a.x-b.x,a.y-b.y,a.z-b.z};
    }
    vec3 operator-(const vec3& b) const
    {
        return {x - b.x, y - b.y, z-b.z};
    }
    vec3& operator-=(const vec3& b)
    {
        x -= b.x;
        y -= b.y;
        z -= b.z;
        return *this;
    }
    vec3& operator*=(const f32 s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    vec3& operator*=(const vec3 b){
        x *= b.x;
        y *= b.y;
        z *= b.z;
        return *this;
    }
    vec3& operator/=(const f32 s)
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }
    vec3& operator/=(const vec3& b){
        x /= b.x;
        y /= b.y;
        z /= b.z;
        return *this;
    }
};

struct vec4 {
    // An array of x, y, z, w
    union
    {
        struct { f32 x, y, z, w; };
        struct { f32 r, g, b, a; };
        struct { f32 s, t, p, q; };
        f32 elements[4];
    };
};

typedef vec4 quat;

typedef union mat4_u {
    f32 data[16];
} mat4;

vec3 tovec3(vec4 a){return vec3{a.x,a.y,a.z};}
vec4 tovec4(vec3 a){return vec4{a.x,a.y,a.z,0};}