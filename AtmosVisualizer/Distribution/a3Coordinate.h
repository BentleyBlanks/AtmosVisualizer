#pragma once
#include "ofMain.h"

class a3Coordinate
{
public:
    a3Coordinate();

    ~a3Coordinate();
    
    void setOrigin(float x, float y);

    // ��ʽ����(pixel)
    void setInterval(float inteval);

    void setAxisLength(float xLength, float yLength);

    // ����һ��Ϊ[0, 1]����Ⱦ
    void drawPiecewise(vector<float> function);

    void drawLinear(vector<float> function);

    void drawAxis();

    void drawPoint(float x, float y);

private:
    friend class a3Distribution1D;

    friend class a3Distribution2D;

    ofVec2f origin;

    float interval;

    float xLength, yLength;

    float xVisualLength, yVisualLength;
};
