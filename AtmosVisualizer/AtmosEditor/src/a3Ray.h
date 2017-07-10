#pragma once

#include "ofMain.h"

class a3Ray
{
public:
    a3Ray(ofVec3f start, ofVec3f end);
    ~a3Ray();

    void draw();

    ofVec3f start, end;
};