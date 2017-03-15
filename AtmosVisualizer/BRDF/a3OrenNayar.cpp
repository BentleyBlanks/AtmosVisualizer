#include "a3OrenNayar.h"

const float INV_PI = 0.31830988618379067154f;

inline float cosTheta(const ofVec3f &w) { return w.z; }
inline float absCosTheta(const ofVec3f &w) { return fabsf(w.z); }
inline float sinTheta2(const ofVec3f &w)
{
    return max(0.f, 1.f - cosTheta(w) * cosTheta(w));
}


inline float sinTheta(const ofVec3f &w)
{
    return sqrtf(sinTheta2(w));
}

inline float cosPhi(const ofVec3f &w)
{
    float sintheta = sinTheta(w);
    if(sintheta == 0.f) return 1.f;
    return ofClamp(w.y / sintheta, -1.f, 1.f);
}


inline float sinPhi(const ofVec3f &w)
{
    float sintheta = sinTheta(w);
    if(sintheta == 0.f) return 0.f;
    return ofClamp(w.x / sintheta, -1.f, 1.f);
}

a3OrenNayar::a3OrenNayar()
{
}

ofVec3f a3OrenNayar::sample(const ofVec3f & wi, const ofVec3f & wo, float * pdf) const
{
    return ofVec3f();
}

ofVec3f a3OrenNayar::evaluate(const ofVec3f & wi, const ofVec3f & wo) const
{
    float sigmaRad = ofDegToRad(sigma);
    float sigmaRad2 = sigmaRad* sigmaRad;

    float A = 1.0f - (sigmaRad2 / (2 * (sigmaRad2 + 0.33)));
    float B = 0.45 * sigmaRad2 / (sigmaRad2 + 0.09);

    float sinThetai = sinTheta(wi);
    float sinThetao = sinTheta(wo);

    float maxcos = 0.f;
    if(sinThetai > 1e-4 && sinThetao > 1e-4)
    {
        float sinPhii = sinPhi(wi);
        float sinPhio = sinPhi(wo);
        float cosPhii = cosPhi(wi);
        float cosPhio = cosPhi(wo);

        float dcos = cosPhii * cosPhio + sinPhii * sinPhio;

        maxcos = max(0.f, dcos);
    }
    // Compute sine and tangent terms of Oren-Nayar model
    float sinalpha, tanbeta;
    if(absCosTheta(wi) > absCosTheta(wo))
    {
        sinalpha = sinThetao;
        tanbeta = sinThetai / absCosTheta(wi);
    }
    else
    {
        sinalpha = sinThetai;
        tanbeta = sinThetao / absCosTheta(wo);
    }
    return R * INV_PI * (A + B * maxcos * sinalpha * tanbeta);
}
