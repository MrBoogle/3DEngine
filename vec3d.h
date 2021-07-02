#pragma once

#ifndef _MAT4X4_
#define _MAT4X4_
#include "mat4x4.h"
#endif // !_VEC3D_

struct mat4x4;

struct vec3d {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;
};

vec3d operator+(vec3d const& v1, vec3d const& v2);

vec3d operator-(vec3d const& v1, vec3d const& v2);

//Dot product
float operator*(vec3d const& v1, vec3d const& v2);

//Scalar multiplication
vec3d operator*(float const& k, vec3d const& v1);

vec3d operator*(vec3d const& v1, float const& k);

//Scalar divison
vec3d operator/(vec3d const& v1, float const& k);

//Cross product
vec3d operator%(vec3d const& v1, vec3d const& v2);

//Returns length of vector
float vecLength(vec3d const& v);

//Returns a normalized unit vector
vec3d normalizeVec(vec3d const& v);

//Matrix multiplication
vec3d operator*(mat4x4 const& m, vec3d const& v1);


