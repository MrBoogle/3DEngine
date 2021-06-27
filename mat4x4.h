#pragma once

struct mat4x4 {
    float m[4][4] = { 0 };
};

mat4x4 makeIdentity();

mat4x4 makeRotX(float fAngleRad);

mat4x4 makeRotY(float fAngleRad);

mat4x4 makeRotZ(float fAngleRad);

mat4x4 makeRot(float gammaX, float betaY, float alphaZ);

mat4x4 makeTranslation(float x, float y, float z);

mat4x4 makeProj(float fFovDeg, float fAspectRatio, float fNear, float fFar);

mat4x4 multMat(mat4x4& m1, mat4x4& m2);
