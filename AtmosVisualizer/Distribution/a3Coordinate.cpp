#include "a3Coordinate.h"

a3Coordinate::a3Coordinate()
{
}

a3Coordinate::~a3Coordinate()
{

}

void a3Coordinate::setOrigin(float x, float y)
{
    origin.set(x, y);
}

void a3Coordinate::setInterval(float interval)
{
    this->interval = interval;
}

void a3Coordinate::setAxisLength(float xLength, float yLength)
{
    this->xLength = xLength;
    this->yLength = yLength;

    xVisualLength = xLength * 0.95;
    yVisualLength = yLength * 0.9;
}


void a3Coordinate::drawAxis()
{
    // Y
    ofDrawArrow(origin, ofVec2f(origin.x, origin.y - yLength), 4);

    // X
    ofDrawArrow(origin, ofVec2f(origin.x + xLength, origin.y), 4);
}

void a3Coordinate::drawPiecewise(vector<float> function)
{
    float max = -1.0f;
    for(int i = 0; i < function.size(); i++)
    {
        if(max < function[i])
            max = function[i];
    }

    // function f(x)
    for(int i = 0; i < function.size(); i++)
    {
        float delta = xVisualLength / function.size();

        float startX = origin.x + i * delta, endX = origin.x + (i + 1) * delta;

        float fx = 0.0f;

        if(max != 0.0f)
            fx = function[i] / max;

        float startY = origin.y - fx * yVisualLength, endY = origin.y - fx * yVisualLength;

        ofDrawLine(ofPoint(startX, startY), ofPoint(endX, endY));
    }
}

void a3Coordinate::drawLinear(vector<float> function)
{
    float max = -1.0f;
    for(int i = 0; i < function.size(); i++)
    {
        if(max < function[i])
            max = function[i];
    }

    // function f(x)
    for(int i = 0; i < function.size() - 1; i++)
    {
        float delta = xVisualLength / function.size();

        float startX = origin.x + i * delta, endX = origin.x + (i + 1) * delta;

        float fx = 0.0f, fx1 = 0.0f;

        if(max != 0.0f)
        {
            fx = function[i] / max;
            fx1 = function[i + 1] / max;
        }

        float startY = origin.y - fx * yVisualLength, endY = origin.y - fx1 * yVisualLength;

        ofDrawLine(ofPoint(startX, startY), ofPoint(endX, endY));
    }
}