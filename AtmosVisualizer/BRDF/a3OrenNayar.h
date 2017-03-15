#pragma once
#include "a3BSDF.h"

class a3OrenNayar :public a3BSDF
{
public:
    a3OrenNayar();

    virtual ofVec3f sample(const ofVec3f& wi, const ofVec3f& wo, float* pdf) const override;

    virtual ofVec3f evaluate(const ofVec3f& wi, const ofVec3f& wo) const override;

    float sigma;
};