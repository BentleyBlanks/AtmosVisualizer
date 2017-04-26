#pragma once

#include "ofMain.h"
#include "DrawUtils.h"
#include "ofxImGui.h"
#include "a3TypeData.h"
#include "messageQueue/a3Message.h"
#include "messageQueue/a3MessageQueueIPC.h"

enum a3PreviewType
{
    A3_PREVIEW_REALTIME,
    A3_PREVIEW_LOCAL,
    A3_PREVIEW_IPC,
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

    void updateMQ();
    void guiDraw();

    // window
    void modelWindow();
    void shapeWindow();
    void lightWindow();
    void viewWindow();
    void cameraWindow();
    void rendererWindow();

    // about window
    void aboutWindow();

    // preview
    void realtimePreview();
    void localPreview();
    void ipcPreview();

    // camera
    ofCamera* getActiveCamera();

    // editor
    ofxImGui::Gui gui; 
    bool openViewWindow, openCameraWindow, openShapeWindow, openLightWindow, openModelWindow, openAboutWindow, openRendererWindow;

    ofImage logo;
    GLuint logoButtonID;

    // preview
    Graph3D* ground;
    ofEasyCam freeCam;
    a3MessageQueueIPC ipcC2S, ipcS2C;
    ofFbo ipcFbo;

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
    a3PreviewType previewType;

    // shape
    vector<a3ShapeData*> shapeList;

    // renderer
    int gridLevel[2];
    bool enableGammaCorrection, enableToneMapping;
    int spp;
};
