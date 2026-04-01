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

KINLINE mat4 mat4::identity() {
    mat4 out_matrix;
    out_matrix.data[0] = 1.0f;
    out_matrix.data[5] = 1.0f;
    out_matrix.data[10] = 1.0f;
    out_matrix.data[15] = 1.0f;
    return out_matrix;
}

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

KINLINE mat4 mat4::perspective(f32 fov_radians, f32 aspect_ratio, f32 near_clip, f32 far_clip) {
    f32 half_tan_fov = ktan(fov_radians * 0.5f);
    mat4 out_matrix;
    out_matrix.data[0] = 1.0f / (aspect_ratio * half_tan_fov);
    out_matrix.data[5] = 1.0f / half_tan_fov;
    out_matrix.data[10] = -((far_clip + near_clip) / (far_clip - near_clip));
    out_matrix.data[11] = -1.0f;
    out_matrix.data[14] = -((2.0f * far_clip * near_clip) / (far_clip - near_clip));
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

KINLINE mat4 mat4::inverse() {
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

KINLINE quat quat::from_axis_angle(vec3 axis, f32 angle, b8 normalize){
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

// Calculates a rotation matrix based on the quaternion and the passed in center point.
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
}


#pragma endregion