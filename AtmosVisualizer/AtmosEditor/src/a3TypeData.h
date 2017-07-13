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
    //string imageName;
    //ofImage image;
    //GLuint imageID;
    int textureIndex;

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
    int materialIndex;

    ofSpherePrimitive* sphere;
    ofPlanePrimitive* plane;
};

struct a3EditorModelData : public a3ModelData
{
    a3EditorModelData(ofxAssimpModelLoader* loader, string modelPath, string name);
    ~a3EditorModelData();

    void draw();

    ofxAssimpModelLoader* model;
    string name;
    int materialIndex;

    bool drawFaces;
    // 0~255
    int previewColor[3];
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

struct a3EditorMaterialData : public a3MaterialData
{
    a3EditorMaterialData();
    ~a3EditorMaterialData() {}

    string name;

    int textureIndex;
};

struct a3EditorTextureData : public a3TextureData
{
    a3EditorTextureData();
    ~a3EditorTextureData() {}

    string name;

    // for image texture
    //ofImage image;
    ofFloatImage image;
    //GLuint imageID;
};