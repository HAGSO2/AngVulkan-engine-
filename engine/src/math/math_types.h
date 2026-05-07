#pragma once

#include "defines.h"
#include "kmath.h"

/*
 * See for more on :
 * OpenGl math library: https://github.com/g-truc/glm
 * Handmade math: https://github.com/HandmadeMath/HandmadeMath/tree/master
 *
 */

// ------------------------------------------
#pragma region Vector 2
// ------------------------------------------

struct vec2 {
    // An array of x, y
    union {
        struct {
            f32 x, y;
        };
        struct {
            f32 r, g;
        };
        struct {
            f32 s, t;
        };
        struct {
            f32 u, v;
        };

        f32 elements[2];
    };

    KSINLINE vec2 zero() { return {0, 0}; }
    KSINLINE vec2 one() { return vec2{1, 1}; }
    KSINLINE vec2 right() { return vec2{1, 0}; }
    KSINLINE vec2 up() { return vec2{0, 1}; }
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

    vec2 operator+(const vec2& b) const {
        return {x + b.x, y + b.y};
    }
    vec2& operator+=(const vec2& b) {
        x += b.x;
        y += b.y;
        return *this;
    }
    KSINLINE vec2 subs(vec2 a, vec2 b) {
        return vec2{a.x - b.x, a.y - b.y};
    }
    vec2 operator-(const vec2& b) const {
        return {x - b.x, y - b.y};
    }
    vec2& operator-=(const vec2& b) {
        x -= b.x;
        y -= b.y;
        return *this;
    }
    vec2& operator*=(const f32 s) {
        x *= s;
        y *= s;
        return *this;
    }
    vec2& operator*=(const vec2 b) {
        x *= b.x;
        y *= b.y;
        return *this;
    }
    vec2& operator/=(const f32 s) {
        x /= s;
        y /= s;
        return *this;
    }
    vec2& operator/=(const vec2& b) {
        x /= b.x;
        y /= b.y;
        return *this;
    }
};

#pragma endregion

// ------------------------------------------
#pragma region Vector 3
// ------------------------------------------

struct vec3 {
    // An array of x, y, z
    union {
        struct {
            f32 x, y, z;
        };
        struct {
            f32 r, g, b;
        };
        struct {
            f32 s, t, p;
        };
        struct {
            f32 u, v, w;
        };
        f32 elements[3];
    };
    KSINLINE vec3 zero() { return vec3{0, 0, 0}; }
    KSINLINE vec3 one() { return vec3{1, 1, 1}; }
    KSINLINE vec3 right() { return vec3{1, 0, 0}; }
    KSINLINE vec3 up() { return vec3{0, 1, 0}; }
    KSINLINE vec3 forward() { return vec3{0, 0, -1}; }
    KSINLINE vec3 back() { return vec3{0, 0, 1}; }
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
    vec3 operator+(const vec3& b) const {
        return {x + b.x, y + b.y, z + b.z};
    }
    vec3& operator+=(const vec3& b) {
        x += b.x;
        y += b.y;
        z += b.z;
        return *this;
    }
    KSINLINE vec3 subs(vec3 a, vec3 b) {
        return vec3{a.x - b.x, a.y - b.y, a.z - b.z};
    }
    vec3 operator-(const vec3& b) const {
        return {x - b.x, y - b.y, z - b.z};
    }
    vec3& operator-=(const vec3& b) {
        x -= b.x;
        y -= b.y;
        z -= b.z;
        return *this;
    }
    vec3& operator*=(const f32 s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    vec3& operator*=(const vec3 b) {
        x *= b.x;
        y *= b.y;
        z *= b.z;
        return *this;
    }
    vec3& operator/=(const f32 s) {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }
    vec3& operator/=(const vec3& b) {
        x /= b.x;
        y /= b.y;
        z /= b.z;
        return *this;
    }
};

#pragma endregion

// ------------------------------------------
#pragma region Vector 4
// ------------------------------------------

struct vec4 {
    // An array of x, y, z, w
    union {
        struct {
            f32 x, y, z, w;
        };
        struct {
            f32 r, g, b, a;
        };
        struct {
            f32 s, t, p, q;
        };
        f32 elements[4];
    };

    KSINLINE vec4 zero() { return vec4{0, 0, 0, 0}; }
    KSINLINE vec4 one() { return vec4{1, 1, 1, 1}; }
    KSINLINE vec4 right() { return vec4{1, 0, 0, 0}; }
    KSINLINE vec4 up() { return vec4{0, 1, 0, 0}; }
    KSINLINE vec4 back() { return vec4{0, 0, 1, 0}; }
    KINLINE f32 length_squared() { return x * x + y * y + z * z + w * w; }
    KINLINE f32 lenght() { return ksqrt(length_squared()); }

    KINLINE void normalize() {
        f32 lenght = ksqrt(length_squared());
        x /= lenght;
        y /= lenght;
        z /= lenght;
        w /= lenght;
    }
    /**
     * @brief Returns the dot product between the provided vectors. Typically used
     * to calculate the difference in direction.
     * For the vec4 works independent from the struct: vec4_dot_f32
     */
    // KINLINE f32 dot(vec4 b);
    KINLINE const b8 compare(vec4 b, f32 tolerance);
    KINLINE f32 distance(vec4 b);

    /**************
     * OPERADORES *
     **************/

    vec4 operator+(const vec4& b) const {
        return {x + b.x, y + b.y, z + b.z, w + b.w};
    }
    vec4& operator+=(const vec4& b) {
        x += b.x;
        y += b.y;
        z += b.z;
        w += b.w;
        return *this;
    }
    KSINLINE vec4 subs(vec4 a, vec4 b) {
        return vec4{a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    }
    vec4 operator-(const vec4& b) const {
        return {x - b.x, y - b.y, z - b.z, w - b.w};
    }
    vec4& operator-=(const vec4& b) {
        x -= b.x;
        y -= b.y;
        z -= b.z;
        w -= b.w;
        return *this;
    }
    vec4& operator*=(const f32 s) {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
        return *this;
    }
    vec4& operator*=(const vec4 b) {
        x *= b.x;
        y *= b.y;
        z *= b.z;
        w *= b.w;
        return *this;
    }
    vec4& operator/=(const f32 s) {
        x /= s;
        y /= s;
        z /= s;
        w /= s;
        return *this;
    }
    vec4& operator/=(const vec4& b) {
        x /= b.x;
        y /= b.y;
        z /= b.z;
        w /= b.w;
        return *this;
    }
};

#pragma endregion

// ------------------------------------------
#pragma region Matrix 4
// ------------------------------------------

struct mat4 {
    f32 data[16];

    KSINLINE mat4 identity() {
        mat4 out_matrix;
        out_matrix.data[0] = 1.0f;
        out_matrix.data[5] = 1.0f;
        out_matrix.data[10] = 1.0f;
        out_matrix.data[15] = 1.0f;
        return out_matrix;
    }
    /**
     * @brief Returns a forward vector relative to the provided matrix.
     *
     * @param matrix The matrix from which to base the vector.
     * @return A 3-component directional vector.
     */
    KINLINE vec3 forward();
    /**
     * @brief Returns a backward vector relative to the provided matrix.
     *
     * @param matrix The matrix from which to base the vector.
     * @return A 3-component directional vector.
     */
    KINLINE vec3 backward();
    /**
     * @brief Returns a upward vector relative to the provided matrix.
     *
     * @param matrix The matrix from which to base the vector.
     * @return A 3-component directional vector.
     */
    KINLINE vec3 up();
    /**
     * @brief Returns a downward vector relative to the provided matrix.
     *
     * @param matrix The matrix from which to base the vector.
     * @return A 3-component directional vector.
     */
    KINLINE vec3 down();
    /**
     * @brief Returns a left vector relative to the provided matrix.
     *
     * @param matrix The matrix from which to base the vector.
     * @return A 3-component directional vector.
     */
    KINLINE vec3 left();
    /**
     * @brief Returns a right vector relative to the provided matrix.
     *
     * @param matrix The matrix from which to base the vector.
     * @return A 3-component directional vector.
     */
    KINLINE vec3 right();
    /**
     * @brief Returns the result of multiplying matrix_0 and matrix_1.
     *
     * @param matrix_0 The first matrix to be multiplied.
     * @param matrix_1 The second matrix to be multiplied.
     * @return The result of the matrix multiplication.
     */
    KSINLINE mat4 multiply(mat4 matrix_0, mat4 matrix_1);
    /**
     * @brief Creates and returns an orthographic projection matrix. Typically used to
     * render flat or 2D scenes.
     *
     * @param left The left side of the view frustum.
     * @param right The right side of the view frustum.
     * @param bottom The bottom side of the view frustum.
     * @param top The top side of the view frustum.
     * @param near_clip The near clipping plane distance.
     * @param far_clip The far clipping plane distance.
     * @return A new orthographic projection matrix.
     */
    KSINLINE mat4 orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near_clip, f32 far_clip);
    /**
     * @brief Creates and returns a perspective matrix. Typically used to render 3d scenes.
     *
     * @param fov_radians The field of view in radians.
     * @param aspect_ratio The aspect ratio.
     * @param near_clip The near clipping plane distance.
     * @param far_clip The far clipping plane distance.
     * @return A new perspective matrix.
     */
    KSINLINE mat4 perspective(f32 fov_radians, f32 aspect_ratio, f32 near_clip, f32 far_clip) {
        f32 half_tan_fov = ktan(fov_radians * 0.5f);
        mat4 out_matrix;
        out_matrix.data[0] = 1.0f / (aspect_ratio * half_tan_fov);
        out_matrix.data[5] = 1.0f / half_tan_fov;
        out_matrix.data[10] = -((far_clip + near_clip) / (far_clip - near_clip));
        out_matrix.data[11] = -1.0f;
        out_matrix.data[14] = -((2.0f * far_clip * near_clip) / (far_clip - near_clip));
        return out_matrix;
    }
    /**
     * @brief Creates and returns a look-at matrix, or a matrix looking
     * at target from the perspective of position.
     *
     * @param position The position of the matrix.
     * @param target The position to "look at".
     * @param up The up vector.
     * @return A matrix looking at target from the perspective of position.
     */
    KSINLINE mat4 look_at(vec3 position, vec3 target, vec3 up);
    /**
     * @brief Returns a transposed copy of the provided matrix (rows->colums)
     *
     * @param matrix The matrix to be transposed.
     * @return A transposed copy of of the provided matrix.
     */
    KINLINE mat4 transposed();
    /**
     * @brief Creates and returns an inverse of the matrix itself.
     *
     * @return A inverted copy of the provided matrix.
     */
    KINLINE mat4 inverse() {
        const f32* m = data;

        f32 t0 = m[10] * m[15];
        f32 t1 = m[14] * m[11];
        f32 t2 = m[6] * m[15];
        f32 t3 = m[14] * m[7];
        f32 t4 = m[6] * m[11];
        f32 t5 = m[10] * m[7];
        f32 t6 = m[2] * m[15];
        f32 t7 = m[14] * m[3];
        f32 t8 = m[2] * m[11];
        f32 t9 = m[10] * m[3];
        f32 t10 = m[2] * m[7];
        f32 t11 = m[6] * m[3];
        f32 t12 = m[8] * m[13];
        f32 t13 = m[12] * m[9];
        f32 t14 = m[4] * m[13];
        f32 t15 = m[12] * m[5];
        f32 t16 = m[4] * m[9];
        f32 t17 = m[8] * m[5];
        f32 t18 = m[0] * m[13];
        f32 t19 = m[12] * m[1];
        f32 t20 = m[0] * m[9];
        f32 t21 = m[8] * m[1];
        f32 t22 = m[0] * m[5];
        f32 t23 = m[4] * m[1];

        mat4 out_matrix;
        f32* o = out_matrix.data;

        o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
        o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
        o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
        o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

        f32 d = 1.0f / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);

        o[0] = d * o[0];
        o[1] = d * o[1];
        o[2] = d * o[2];
        o[3] = d * o[3];
        o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
        o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
        o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
        o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
        o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
        o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
        o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
        o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
        o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
        o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
        o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
        o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));

        return out_matrix;
    }
    KSINLINE mat4 translation(vec3 position) {
        mat4 out_matrix = mat4::identity();
        out_matrix.data[12] = position.x;
        out_matrix.data[13] = position.y;
        out_matrix.data[14] = position.z;
        return out_matrix;
    }
    KSINLINE mat4 scale(vec3 scale);
    KSINLINE mat4 euler_x(f32 angle_radians);
    KSINLINE mat4 euler_y(f32 angle_radians);
    KSINLINE mat4 euler_z(f32 angle_radians);
    KSINLINE mat4 euler_xyz(f32 x_radians, f32 y_radians, f32 z_radians);
};

#pragma endregion

// ------------------------------------------
#pragma region Quaternion
// ------------------------------------------

struct quat : vec4 {
    KSINLINE quat identity() { return quat{0, 0, 0, 1}; }
    KINLINE f32 normal() { return vec4::length_squared(); }
    KINLINE void normalize() { vec4::normalize(); }
    KINLINE quat conjugate() { return {-x, -y - z, w}; };
    KINLINE void inverse() { (this->conjugate()).normalize(); }
    KSINLINE quat mult(quat q_0, quat q_1) {
        quat out_quaternion;

        out_quaternion.x = q_0.x * q_1.w +
                           q_0.y * q_1.z -
                           q_0.z * q_1.y +
                           q_0.w * q_1.x;

        out_quaternion.y = -q_0.x * q_1.z +
                           q_0.y * q_1.w +
                           q_0.z * q_1.x +
                           q_0.w * q_1.y;

        out_quaternion.z = q_0.x * q_1.y -
                           q_0.y * q_1.x +
                           q_0.z * q_1.w +
                           q_0.w * q_1.z;

        out_quaternion.w = -q_0.x * q_1.x -
                           q_0.y * q_1.y -
                           q_0.z * q_1.z +
                           q_0.w * q_1.w;

        return out_quaternion;
    }
    KINLINE f32 dot(quat q_1);
    KSINLINE quat from_axis_angle(vec3 axis, f32 angle, b8 normalize) {
        const f32 half_angle = 0.5f * angle;
        f32 s = ksin(half_angle);
        f32 c = kcos(half_angle);

        quat q = (quat){s * axis.x, s * axis.y, s * axis.z, c};
        if (normalize) {
            q.normalize();
            return q;
        }
        return q;
    }
    KINLINE quat slerp(quat q_1, f32 percentage);

    quat operator*(const quat b) { return mult(*this, b); }
};

#pragma endregion

struct vertex_3d {
    vec3 position;
    vec2 texcoord;
};

struct vertex_2d {
    vec2 position;
    vec2 texcoord;
};

KSINLINE vec3 to_vec3(vec4 a) { return vec3{a.x, a.y, a.z}; }
KSINLINE vec4 to_vec4(vec3 a) { return vec4{a.x, a.y, a.z, 0}; }
KSINLINE mat4 to_mat4(quat a);
/**
 * @brief Calculates a rotation matrix based on the quaternion and the passed in center point.
 *
 * @param q The quaternion representing the rotation.
 * @param center The center point around which to rotate.
 * @return The resulting rotation matrix.
 */
KSINLINE mat4 to_rotation_mat4(quat q, vec3 center) {
    mat4 out_matrix;

    f32* o = out_matrix.data;
    o[0] = (q.x * q.x) - (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[1] = 2.0f * ((q.x * q.y) + (q.z * q.w));
    o[2] = 2.0f * ((q.x * q.z) - (q.y * q.w));
    o[3] = center.x - center.x * o[0] - center.y * o[1] - center.z * o[2];

    o[4] = 2.0f * ((q.x * q.y) - (q.z * q.w));
    o[5] = -(q.x * q.x) + (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
    o[6] = 2.0f * ((q.y * q.z) + (q.x * q.w));
    o[7] = center.y - center.x * o[4] - center.y * o[5] - center.z * o[6];

    o[8] = 2.0f * ((q.x * q.z) + (q.y * q.w));
    o[9] = 2.0f * ((q.y * q.z) - (q.x * q.w));
    o[10] = -(q.x * q.x) - (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
    o[11] = center.z - center.x * o[8] - center.y * o[9] - center.z * o[10];

    o[12] = 0.0f;
    o[13] = 0.0f;
    o[14] = 0.0f;
    o[15] = 1.0f;

    return out_matrix;
};
KSINLINE f32 vec4_dot_f32(
    f32 a0, f32 a1, f32 a2, f32 a3,
    f32 b0, f32 b1, f32 b2, f32 b3);
/**
 * @brief Converts provided degrees to radians.
 *
 * @param degrees The degrees to be converted.
 * @return The amount in radians.
 */
KSINLINE f32 deg_to_rad(f32 degrees) {
    return degrees * K_DEG2RAD_MULTIPLIER;
}

/**
 * @brief Converts provided radians to degrees.
 *
 * @param radians The radians to be converted.
 * @return The amount in degrees.
 */
KSINLINE f32 rad_to_deg(f32 radians) {
    return radians * K_RAD2DEG_MULTIPLIER;
}