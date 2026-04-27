#include "math/math_types.h"
#include "kmath.h"

// ------------------------------------------
#pragma region Vector 2
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

#pragma endregion

// ------------------------------------------
#pragma region Vector 3
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

#pragma endregion

// ------------------------------------------
#pragma region Vector 4
// ------------------------------------------



KSINLINE f32 vec4_dot_f32(
    f32 a0, f32 a1, f32 a2, f32 a3,
    f32 b0, f32 b1, f32 b2, f32 b3) {
    f32 p;
    p =
        a0 * b0 +
        a1 * b1 +
        a2 * b2 +
        a3 * b3;
    return p;
}

KINLINE const b8 vec4::compare(vec4 b, f32 tolerance) {
    if(kabs(w - b.w) > tolerance)
        return FALSE;
    if(kabs(x - b.x) > tolerance)
        return FALSE;
    if(kabs(y - b.y) > tolerance)
        return FALSE;
    if (kabs(z - b.z) > tolerance)
        return FALSE;
    
    return TRUE;
}

KINLINE f32 vec4::distance(vec4 b) {
    return subs(*this,b).lenght();
}

#pragma endregion

// ------------------------------------------
#pragma region Matrix 4
// ------------------------------------------

KINLINE vec3 mat4::backward() {
    vec3 backward;
    backward.x = data[2];
    backward.y = data[6];
    backward.z = data[10];
    backward.normalize();
    return backward;
}

KINLINE vec3 mat4::up() {
    vec3 up;
    up.x = data[1];
    up.y = data[5];
    up.z = data[9];
    up.normalize();
    return up;
}

KINLINE vec3 mat4::right() {
    vec3 right;
    right.x = data[0];
    right.y = data[4];
    right.z = data[8];
    right.normalize();
    return right;
}

KINLINE mat4 mat4::multiply(mat4 matrix_0, mat4 matrix_1) {
    mat4 out_matrix = identity();

    const f32* m1_ptr = matrix_0.data;
    const f32* m2_ptr = matrix_1.data;
    f32* dst_ptr = out_matrix.data;

    for (i32 i = 0; i < 4; ++i) {
        for (i32 j = 0; j < 4; ++j) {
            *dst_ptr =
                m1_ptr[0] * m2_ptr[0 + j] +
                m1_ptr[1] * m2_ptr[4 + j] +
                m1_ptr[2] * m2_ptr[8 + j] +
                m1_ptr[3] * m2_ptr[12 + j];
            dst_ptr++;
        }
        m1_ptr += 4;
    }
    return out_matrix;
}

KINLINE mat4 mat4::orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near_clip, f32 far_clip) {
    mat4 out_matrix = mat4::identity();

    f32 lr = 1.0f / (left - right);
    f32 bt = 1.0f / (bottom - top);
    f32 nf = 1.0f / (near_clip - far_clip);

    out_matrix.data[0] = -2.0f * lr;
    out_matrix.data[5] = -2.0f * bt;
    out_matrix.data[10] = 2.0f * nf;

    out_matrix.data[12] = (left + right) * lr;
    out_matrix.data[13] = (top + bottom) * bt;
    out_matrix.data[14] = (far_clip + near_clip) * nf;
    return out_matrix;
}


KINLINE mat4 mat4::look_at(vec3 position, vec3 target, vec3 up) {
    mat4 out_matrix;
    vec3 z_axis;
    z_axis.x = target.x - position.x;
    z_axis.y = target.y - position.y;
    z_axis.z = target.z - position.z;

    z_axis.normalize();
    vec3 x_axis = vec3::cross(z_axis,vec3::up());
    x_axis.normalize();
    vec3 y_axis = vec3::cross(x_axis, z_axis);

    out_matrix.data[0] = x_axis.x;
    out_matrix.data[1] = y_axis.x;
    out_matrix.data[2] = -z_axis.x;
    out_matrix.data[3] = 0;
    out_matrix.data[4] = x_axis.y;
    out_matrix.data[5] = y_axis.y;
    out_matrix.data[6] = -z_axis.y;
    out_matrix.data[7] = 0;
    out_matrix.data[8] = x_axis.z;
    out_matrix.data[9] = y_axis.z;
    out_matrix.data[10] = -z_axis.z;
    out_matrix.data[11] = 0;
    out_matrix.data[12] = -x_axis.dot(position);
    out_matrix.data[13] = -y_axis.dot(position);
    out_matrix.data[14] = z_axis.dot(position);
    out_matrix.data[15] = 1.0f;

    return out_matrix;
}

KINLINE mat4 mat4::transposed() {
    mat4 out_matrix = mat4::identity();
    out_matrix.data[0] = data[0];
    out_matrix.data[1] = data[4];
    out_matrix.data[2] = data[8];
    out_matrix.data[3] = data[12];
    out_matrix.data[4] = data[1];
    out_matrix.data[5] = data[5];
    out_matrix.data[6] = data[9];
    out_matrix.data[7] = data[13];
    out_matrix.data[8] = data[2];
    out_matrix.data[9] = data[6];
    out_matrix.data[10] = data[10];
    out_matrix.data[11] = data[14];
    out_matrix.data[12] = data[3];
    out_matrix.data[13] = data[7];
    out_matrix.data[14] = data[11];
    out_matrix.data[15] = data[15];
    return out_matrix;
}

// KINLINE mat4 mat4::translation(vec3 position) {
    
// }

KINLINE mat4 mat4::scale(vec3 scale) {
    mat4 out_matrix = mat4::identity();
    out_matrix.data[0] = scale.x;
    out_matrix.data[5] = scale.y;
    out_matrix.data[10] = scale.z;
    return out_matrix;
}

KINLINE mat4 mat4::euler_x(f32 angle_radians) {
    mat4 out_matrix = mat4::identity();
    f32 c = kcos(angle_radians);
    f32 s = ksin(angle_radians);

    out_matrix.data[5] = c;
    out_matrix.data[6] = s;
    out_matrix.data[9] = -s;
    out_matrix.data[10] = c;
    return out_matrix;
}

KINLINE mat4 mat4::euler_y(f32 angle_radians) {
    mat4 out_matrix = mat4::identity();
    f32 c = kcos(angle_radians);
    f32 s = ksin(angle_radians);

    out_matrix.data[0] = c;
    out_matrix.data[2] = -s;
    out_matrix.data[8] = s;
    out_matrix.data[10] = c;
    return out_matrix;
}

KINLINE mat4 mat4::euler_z(f32 angle_radians) {
    mat4 out_matrix = mat4::identity();

    f32 c = kcos(angle_radians);
    f32 s = ksin(angle_radians);

    out_matrix.data[0] = c;
    out_matrix.data[1] = s;
    out_matrix.data[4] = -s;
    out_matrix.data[5] = c;
    return out_matrix;
}

KINLINE mat4 mat4::euler_xyz(f32 x_radians, f32 y_radians, f32 z_radians) {
    mat4 rx = mat4::euler_x(x_radians);
    mat4 ry = mat4::euler_y(y_radians);
    mat4 rz = mat4::euler_z(z_radians);
    mat4 out_matrix = mat4::multiply(rx, ry);
    out_matrix = mat4::multiply(out_matrix, rz);
    return out_matrix;
}

#pragma endregion

// ------------------------------------------
#pragma region Quaternion
// ------------------------------------------

// KINLINE quat quat::mult(quat q_0, quat q_1) {
    
// }

KINLINE f32 quat::dot(quat q_1) {
    return x * q_1.x +
           y * q_1.y +
           z * q_1.z +
           w * q_1.w;
}

KINLINE quat quat::slerp(quat q_1, f32 percentage) {
    quat out_quaternion;
    // Source: https://en.wikipedia.org/wiki/Slerp
    // Only unit quaternions are valid rotations.
    // Normalize to avoid undefined behavior.
    quat v0 = *this;
    v0.normalize();
    quat v1 = q_1;
    v1.normalize();

    // Compute the cosine of the angle between the two vectors.
    f32 dot = v0.dot(v1);

    // If the dot product is negative, slerp won't take
    // the shorter path. Note that v1 and -v1 are equivalent when
    // the negation is applied to all four components. Fix by
    // reversing one quaternion.
    if (dot < 0.0f) {
        v1.x = -v1.x;
        v1.y = -v1.y;
        v1.z = -v1.z;
        v1.w = -v1.w;
        dot = -dot;
    }

    const f32 DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD) {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.
        out_quaternion = (quat){
            v0.x + ((v1.x - v0.x) * percentage),
            v0.y + ((v1.y - v0.y) * percentage),
            v0.z + ((v1.z - v0.z) * percentage),
            v0.w + ((v1.w - v0.w) * percentage)};

        out_quaternion.normalize();
        return out_quaternion;
    }

    // Since dot is in range [0, DOT_THRESHOLD], acos is safe
    f32 theta_0 = kacos(dot);          // theta_0 = angle between input vectors
    f32 theta = theta_0 * percentage;  // theta = angle between v0 and result
    f32 sin_theta = ksin(theta);       // compute this value only once
    f32 sin_theta_0 = ksin(theta_0);   // compute this value only once

    f32 s0 = kcos(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
    f32 s1 = sin_theta / sin_theta_0;

    return (quat){
        (v0.x * s0) + (v1.x * s1),
        (v0.y * s0) + (v1.y * s1),
        (v0.z * s0) + (v1.z * s1),
        (v0.w * s0) + (v1.w * s1)};
}

KINLINE mat4 quat_to_mat4(quat q) {
    mat4 out_matrix = mat4::identity();

    // https://stackoverflow.com/questions/1556260/convert-quaternion-rotation-to-rotation-matrix

    quat n = q;
    n.normalize();

    out_matrix.data[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
    out_matrix.data[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
    out_matrix.data[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;

    out_matrix.data[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
    out_matrix.data[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
    out_matrix.data[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;

    out_matrix.data[8] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
    out_matrix.data[9] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
    out_matrix.data[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

    return out_matrix;
}

KSINLINE mat4 to_mat4(quat q){
    mat4 out_matrix = mat4::identity();

    // https://stackoverflow.com/questions/1556260/convert-quaternion-rotation-to-rotation-matrix

    quat n = q;
    n.normalize();

    out_matrix.data[0] = 1.0f - 2.0f * n.y * n.y - 2.0f * n.z * n.z;
    out_matrix.data[1] = 2.0f * n.x * n.y - 2.0f * n.z * n.w;
    out_matrix.data[2] = 2.0f * n.x * n.z + 2.0f * n.y * n.w;

    out_matrix.data[4] = 2.0f * n.x * n.y + 2.0f * n.z * n.w;
    out_matrix.data[5] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.z * n.z;
    out_matrix.data[6] = 2.0f * n.y * n.z - 2.0f * n.x * n.w;

    out_matrix.data[8] = 2.0f * n.x * n.z - 2.0f * n.y * n.w;
    out_matrix.data[9] = 2.0f * n.y * n.z + 2.0f * n.x * n.w;
    out_matrix.data[10] = 1.0f - 2.0f * n.x * n.x - 2.0f * n.y * n.y;

    return out_matrix;
}

#pragma endregion