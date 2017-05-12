#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "messageQueue/a3Message.h"

ofVec3f a3Float3ToVec3(float* f3);

void a3Float3Set(float* f3, float t1, float t2, float t3);

void a3Float3Set(float* f3, ofVec3f v);

struct a3ShapeData;
struct a3EditorLightData : public a3LightData
{
public:
    a3EditorLightData();
    ~a3EditorLightData();

    void updatePosition();

    void draw();

    string name;

    // --------------------infinite light--------------------
    string imageName;
    ofImage image;
    GLuint imageID;

    float coneHeight;

    // display
    ofNode* node;
private:
    void drawNode();
};

struct a3EditorShapeData : public a3ShapeData
{
    a3EditorShapeData();
    ~a3EditorShapeData();

    void draw();

    string name;

    ofSpherePrimitive* sphere;
    ofPlanePrimitive* plane;
};

struct a3EditorModelData : public a3ModelData
{
    a3EditorModelData(ofxAssimpModelLoader* loader, string modelPath, string name) 
        :model(loader), name(name)
    {
        if(modelPath.length() <= A3_ADDRESS_PATH_LENGTH)
            strcpy(path, modelPath.c_str());
    }

    ~a3EditorModelData();

    void draw();

    ofxAssimpModelLoader* model;
    string name;
};

struct a3EditorCameraData : public a3CameraData
{
    a3EditorCameraData(ofCamera* cam, string name);
    ~a3EditorCameraData();

    void draw();

    ofCamera* camera;
    //ofEasyCam* camera;
    string name;

    // width height
    ofVec2f dimension;

    float distance;
    bool lockMouseInput;
    bool active;
};