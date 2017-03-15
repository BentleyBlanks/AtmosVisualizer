#pragma once
#include "a3BSDF.h"

class a3Diffuse :public a3BSDF
{
public:
    a3Diffuse();

    virtual ofVec3f sample(const ofVec3f& wi, const ofVec3f& wo, float* pdf) const override;

    virtual ofVec3f evaluate(const ofVec3f& wi, const ofVec3f& wo) const override;
};