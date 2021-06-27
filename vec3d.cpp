#include "vec3d.h"
#include <algorithm>

vec3d operator+(vec3d const& v1, vec3d const& v2) {
    vec3d res;
    res.x = v1.x + v2.x;
    res.y = v1.y + v2.y;
    res.z = v1.z + v2.z;
    res.w = v1.w + v2.w;
    return res;
}


vec3d operator-(vec3d const& v1, vec3d const& v2) {
    vec3d res;
    res.x = v1.x - v2.x;
    res.y = v1.y - v2.y;
    res.z = v1.z - v2.z;
    res.w = v1.w - v2.w;
    return res;
}

//Dot product
float operator*(vec3d const& v1, vec3d const& v2) {
   
    return v1.x * v2.x + v1.y * v2.y  + v1.z * v2.z;
}


//Scalar multiplication
vec3d operator*(float const& k, vec3d const& v1) {
    vec3d res;
    res.x = v1.x * k;
    res.y = v1.y * k;
    res.z = v1.z * k;
    res.w = v1.w * k;
    return res;
}

vec3d operator*(vec3d const& v1, float const& k) {
    vec3d res;
    res.x = v1.x * k;
    res.y = v1.y * k;
    res.z = v1.z * k;
    res.w = v1.w * k;
    return res;
}

//Scalar division
vec3d operator/(vec3d const& v1, float const& k) {
    vec3d res;
    res.x = v1.x / k;
    res.y = v1.y / k;
    res.z = v1.z / k;
    res.w = v1.w / k;
    return res;
}

//Cross product
vec3d operator%(vec3d const& v1, vec3d const& v2) {
    vec3d v;
    v.x = v1.y * v2.z - v1.z * v2.y;
    v.y = v1.z * v2.x - v1.x * v2.z;
    v.z = v1.x * v2.y - v1.y * v2.x;

    return v;
}

//Get vector length
float vecLength(vec3d const& v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3d normalizeVec(vec3d const& v) {
    float length = vecLength(v);
    return { v.x / length, v.y / length, v.z / length };
}

//Matrix multiplication
vec3d operator*(mat4x4 const& m, vec3d const& v1) {
    vec3d v;
    v.x = v1.x * m.m[0][0] + v1.y * m.m[1][0] + v1.z * m.m[2][0] + v1.w * m.m[3][0];
    v.y = v1.x * m.m[0][1] + v1.y * m.m[1][1] + v1.z * m.m[2][1] + v1.w * m.m[3][1];
    v.z = v1.x * m.m[0][2] + v1.y * m.m[1][2] + v1.z * m.m[2][2] + v1.w * m.m[3][2];
    v.w = v1.x * m.m[0][3] + v1.y * m.m[1][3] + v1.z * m.m[2][3] + v1.w * m.m[3][3];
    return v;
}