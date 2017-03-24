#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "Triangle.h"

//struct Triangle;
class Shape
{
public:
    Shape();
    ~Shape();

    bool load(const char* path);

    // ��ȡ�����������
    std::vector<ofVec3f>* getPointsWithZ(float z);

    ofPolyline* getPolylineWithZ(float z);

    void draw();

    ofxAssimpModelLoader model;
    std::vector<Triangle*> triangles;
};