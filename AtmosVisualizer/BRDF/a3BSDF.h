#pragma once
#include "ofMain.h"

class a3BSDF
{
public:
    a3BSDF();

    // ��������ⷽ�򣬸����ཻ����Ϣ����������ɳ��䷽��
    // �ȼ���PBRT�е�sample_f
    virtual ofVec3f sample(const ofVec3f& wi, const ofVec3f& wo, float* pdf) const = 0;

    // ����������䷽�򣬼����ڸ�������Լ��ཻ����Ϣ����¼���bsdf
    // �ȼ���PBRT�е�f
    virtual ofVec3f evaluate(const ofVec3f& wi, const ofVec3f& wo) const = 0;

    // �Ƿ�ΪDirac Delta Distribution
    bool isDelta;

    // ������
    ofVec3f R;
};
