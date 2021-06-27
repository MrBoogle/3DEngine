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

mat4x4 makeTranslation(float x, float y, float z) {
    mat4x4 m;
    m.m[0][0] = 1.0f;
    m.m[1][1] = 1.0f;
    m.m[2][2] = 1.0f;
    m.m[3][3] = 1.0f;
    m.m[3][0] = x;
    m.m[3][1] = y;
    m.m[3][2] = z;
    return m;
}

mat4x4 makeProj(float fFovDeg, float fAspectRatio, float fNear, float fFar) {
    float fFovRad = 1.0f / tanf(fFovDeg * 0.5f / 180.0f * 3.14159f);
    mat4x4 m;
    m.m[0][0] = fAspectRatio * fFovRad;
    m.m[1][1] = fFovRad;
    m.m[2][2] = fFar / (fFar - fNear);
    m.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    m.m[2][3] = 1.0f;
    m.m[3][3] = 0.0f;
    return m;
}

mat4x4 multMat(mat4x4& m1, mat4x4& m2) {
    mat4x4 mat;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            mat.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
        }
    }
    return mat;
}