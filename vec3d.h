#pragma once

#include "mat4x4.h"

struct vec3d {
    float x, y, z, w;
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

//Returns a normalized vector
vec3d normalizeVec(vec3d const& v);

//Matrix multiplication
vec3d operator*(mat4x4 const& m, vec3d const& v1);

