#pragma once
#include "ofMain.h"

class a3BSDF
{
public:
    a3BSDF();

    // 给定入射光方向，根据相交点信息随机采样生成出射方向
    // 等价于PBRT中的sample_f
    virtual ofVec3f sample(const ofVec3f& wi, const ofVec3f& wo, float* pdf) const = 0;

    // 给定入射出射方向，计算在给定方向对及相交点信息情况下计算bsdf
    // 等价于PBRT中的f
    virtual ofVec3f evaluate(const ofVec3f& wi, const ofVec3f& wo) const = 0;

    // 是否为Dirac Delta Distribution
    bool isDelta;

    // 反射率
    ofVec3f R;
};
