#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

enum a3ShapeType
{
    A3_SHAPETYPE_SPHERE = 0,
    A3_SHAPETYPE_DISK,
    A3_SHAPETYPE_PLANE,
    A3_SHAPETYPE_TRIANGLE,
    A3_SHAPETYPE_INFINITE_PLANE,
};

enum a3LightType
{
    A3_LIGHTTYPE_POINT = 0,
    A3_LIGHTTYPE_AREA,
    A3_LIGHTTYPE_INFINITE_AREA,
    A3_LIGHTTYPE_SPOT,
};

struct a3ShapeData;
struct a3LightData
{
public:
    a3LightData();
    ~a3LightData();

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

struct a3ShapeData
{
    a3ShapeData();
    ~a3ShapeData();

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

struct a3ModelData
{
    a3ModelData(ofxAssimpModelLoader* loader, string path, string name)
        :model(loader), path(path), name(name) {}
    ~a3ModelData();

    void draw();

    ofxAssimpModelLoader* model;
    string path;
    string name;
};

struct a3CameraData
{
    a3CameraData(ofCamera* cam, string name);
    ~a3CameraData();

    void draw();

    ofCamera* camera;
    //ofEasyCam* camera;
    string name;

    ofVec3f origin;
    ofVec3f lookAt;
    ofVec3f up;

    float fov;
    // width height
    ofVec2f dimension;

    float distance;
    bool lockMouseInput;
    bool active;
};

string getShapeTypeName(a3ShapeType type);

string getLightTypeName(a3LightType type);