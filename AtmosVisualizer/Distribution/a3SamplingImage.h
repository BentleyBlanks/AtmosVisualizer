#pragma once
#include "ofMain.h"

class a3SamplingImage
{
public:
    a3SamplingImage(vector<float> data, ofFloatImage image);

    ~a3SamplingImage();

    void setOrigin(int x, int y);

    void setDisplayDimension(int displayWidth, int displayHeight);

    void drawSamples(vector<ofVec2f> samples);

    void drawImageData();

    void drawImage();
private:
    ofVec2f origin;

    ofFloatImage image;

    // 显示宽高
    int displayWidth, displayHeight;

    int displayIntervalX, displayIntervalY;

    int width, height;

    vector<float> data;
};
