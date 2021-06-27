#include "mat4x4.h"
#include <algorithm>

mat4x4 makeIdentity() {
    mat4x4 m;
    m.m[0][0] = 1.0f;
    m.m[1][1] = 1.0f;
    m.m[2][2] = 1.0f;
    m.m[3][3] = 1.0f;

    return m;
}

mat4x4 makeRotX(float fAngleRad) {
    mat4x4 m;
    m.m[0][0] = 1.0f;
    m.m[1][1] = cosf(fAngleRad);
    m.m[2][1] = -sinf(fAngleRad);
    m.m[1][2] = sinf(fAngleRad);
    m.m[2][2] = cosf(fAngleRad);
    m.m[3][3] = 1.0f;
    return m;
}

mat4x4 makeRotY(float fAngleRad) {
    mat4x4 m;
    m.m[0][0] = cosf(fAngleRad);
    m.m[2][0] = sinf(fAngleRad);
    m.m[1][1] = 1.0f;
    m.m[2][2] = cosf(fAngleRad);
    m.m[0][2] = -sinf(fAngleRad);
    m.m[3][3] = 1.0f;
    return m;
}

mat4x4 makeRotZ(float fAngleRad) {
    mat4x4 m;
    m.m[0][0] = cosf(fAngleRad);
    m.m[1][0] = -sinf(fAngleRad);
    m.m[0][1] = sinf(fAngleRad);
    m.m[1][1] = cosf(fAngleRad);
    m.m[3][3] = 1.0f;
    return m;
}

mat4x4 makeRot(float gammaX, float betaY, float alphaZ) {
    mat4x4 m;
    m.m[0][0] = cosf(alphaZ) * cosf(betaY);
    m.m[1][0] = cosf(alphaZ) * sinf(betaY) * sinf(gammaX) - sinf(alphaZ) * cosf(gammaX);
    m.m[2][0] = cosf(alphaZ) * sinf(betaY) * cosf(gammaX) + sinf(alphaZ) * sinf(gammaX);

    m.m[0][1] = sinf(alphaZ) * cosf(betaY);
    m.m[1][1] = sinf(alphaZ) * sinf(betaY) * sinf(gammaX) + cosf(alphaZ) * cosf(gammaX);
    m.m[2][1] = sinf(alphaZ) * sinf(betaY) * cosf(gammaX) - cosf(alphaZ) * sinf(gammaX);

    m.m[0][2] = -sinf(betaY);
    m.m[1][2] = cosf(betaY) * sinf(gammaX);
    m.m[2][2] = cosf(betaY) * cosf(gammaX);

    m.m[3][3] = 1.0f;

    return m;
}