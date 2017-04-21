#pragma once

#include "ofMain.h"
#include "DrawUtils.h"
#include "ofxImGui.h"
#include "ofxAssimpModelLoader.h"

enum a3ShapeType
{
    SPHERE = 0,
    DISK,
    PLANE,
    TRIANGLE,
    INFINITE_PLANE,
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

class ofApp : public ofBaseApp
{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void guiDraw();

    void modelWindow();
    void shapeWindow();
    void lightWindow();
    void viewWindow();
    void cameraWindow();

    // about window
    void aboutWindow();

    // camera
    ofCamera* getActiveCamera();

    // editor
    ofxImGui::Gui gui; 
    bool openViewWindow, openCameraWindow, openShapeWindow, openLightWindow, openModelWindow, openAboutWindow;

    ofImage logo;
    GLuint logoButtonID;

    // preview
    Graph3D* ground;
  
    ofEasyCam freeCam;

    // models
    vector<a3ModelData*> modelList;
    
    // cameras
    vector<a3CameraData*> cameraList;
    bool lockMouseInput;
    int activeCameraIndex;

    // view mode
    // true freecam; false userdefined camera
    bool freeCamPreview;

    // shape
    vector<a3ShapeData*> shapeList;
};
