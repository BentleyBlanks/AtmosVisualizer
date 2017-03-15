#include "a3Distribution2D.h"
#include "a3Distribution1D.h"

a3Distribution2D::a3Distribution2D(std::vector<float> data, int _width, int _height)
{
    if(_width * _height != data.size())
    {
        // a3Log
    }

    width = _width;
    height = _height;

    // ��ʼ���������ʷֲ�
    // p(u|v) = f[u, v]/(\frac{1}{width}*\sum_i f[u_i, v])
    for(int i = 0; i < height; i++)
    {
        std::vector<float> row(data.begin() + i * width, data.begin() + (i + 1) * width);

        a3Distribution1D* conditional = new a3Distribution1D(row);

        conditionalDensity.push_back(conditional);
    }

    // ��Ե���ʷֲ�(��λ��ռ���л��ֵı���)
    std::vector<float> marginalDensityValue; 
    marginalDensityValue.reserve(height);

    // �л���ֵ�������������ܶ��м���(���л���ռȫ�����)
    for(int i = 0; i < height; i++)
        marginalDensityValue.push_back(conditionalDensity[i]->functionIntegral);

    marginalDensity = new a3Distribution1D(marginalDensityValue);
}

a3Distribution2D::~a3Distribution2D()
{
    // ���ͷ�
    for(int i = 0; i < conditionalDensity.size(); i++)
    {
        delete conditionalDensity[i];
        conditionalDensity[i] = NULL;
    }

    conditionalDensity.clear();
    
    delete marginalDensity;
    marginalDensity = NULL;
}

void a3Distribution2D::sampleContinuous(float u0, float u1, float *x0, float *x1, float *pdf)
{
    if(u0 && u1)
    {
        // a3Log
    }

    float pdfMarginal = 0.0f, pdfConditional = 0.0f;

    int index = 0;

    *x1 = marginalDensity->sampleContinuous(u0, &pdfMarginal, &index);

    // �ɱ�Եȷ����ָ����
    *x0 = conditionalDensity[index]->sampleContinuous(u1, &pdfConditional);

    // ���� = ���� * ��Ե
    *pdf = pdfConditional * pdfMarginal;
}
