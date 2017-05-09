#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "messageQueue/a3Message.h"

struct a3ShapeData;
struct a3EditorLightData
{
public:
    a3EditorLightData();
    ~a3EditorLightData();

    void updatePosition();
    //void updateCone();

    void draw();

    string name;
    a3LightType type;

    // all 
    ofVec3f emission;

    // --------------------area--------------------
    a3ShapeData* shape;

    // --------------------infinite light--------------------
    string imageName, imagePath;
    ofImage image;
    GLuint imageID;

    // --------------------point spot--------------------
    ofVec3f position;

    // --------------------spot--------------------
    ofVec3f direction;

    // 圆锥与中心线夹角(弧度)
    float coneAngle;

    // 开始产生半影的角度(弧度)
    float falloffStart;

    float coneHeight;

    // display
    ofNode* node;
    //ofConePrimitive* cone;

private:
    void drawNode();
};

struct a3EditorShapeData
{
    a3EditorShapeData();
    ~a3EditorShapeData();

    void draw();

    string name;
    a3ShapeType type;

    // sphere disk
    float radius;

    // common
    ofVec3f position;

    // disk plane infinite_plane
    ofVec3f normal;

    // plane
    float width, height;

    ofSpherePrimitive* sphere;
    ofPlanePrimitive* plane;
};

struct a3EditorModelData
{
    a3EditorModelData(ofxAssimpModelLoader* loader, string path, string name)
        :model(loader), path(path), name(name) {}
    ~a3EditorModelData();

    void draw();

    ofxAssimpModelLoader* model;
    string path;
    string name;
};

struct a3EditorCameraData
{
    a3EditorCameraData(ofCamera* cam, string name);
    ~a3EditorCameraData();

    void draw();

    ofCamera* camera;
    //ofEasyCam* camera;
    string name;

    ofVec3f origin;
    ofVec3f lookAt;
    ofVec3f up;

    float fov;
    float focalDistance, lensRaidus;
    // width height
    ofVec2f dimension;

    float distance;
    bool lockMouseInput;
    bool active;
};

string getShapeTypeName(a3ShapeType type);

string getLightTypeName(a3LightType type);