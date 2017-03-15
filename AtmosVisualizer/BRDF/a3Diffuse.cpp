#include "a3Diffuse.h"

a3Diffuse::a3Diffuse() : a3BSDF()
{
}

ofVec3f a3Diffuse::sample(const ofVec3f & wi, const ofVec3f & wo, float * pdf) const
{
    return ofVec3f();
}

ofVec3f a3Diffuse::evaluate(const ofVec3f & wi, const ofVec3f & wo) const
{
    return R * 1 / PI;
}
