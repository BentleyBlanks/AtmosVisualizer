#pragma once

#include "ofMain.h"
#include "DrawUtils.h"
#include "ofxImGui.h"
#include "a3TypeData.h"

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

    // light
    vector<a3LightData*> lightList;

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
