#pragma once

#include "ofMain.h"
#include "DrawUtils.h"

// imgui
#include "ofxImGui.h"
#include "imgui_dock.h"

#include "a3Ray.h"
#include "a3TypeData.h"
#include "messageQueue/a3Message.h"
#include "messageQueue/a3MessageQueueIPC.h"
#include "a3EasyCam.h"

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

    void guiSetup();
    void guiDraw();
    void debugDraw();
    void updateMQ();
    void startRenderingProvess(bool isDebug);
    void sendInitMessage();

    // window
    void modelWindow();
    void shapeWindow();
    void lightWindow();
    void cameraWindow();
    void rendererWindow();
    void materialWindow();
    void textureWindow();

    // preview
    void realtimePreview();

    // camera
    ofCamera* getActiveCamera();
    a3EditorCameraData* getActiveCameraData();
    ofFbo* getPreviewFbo();

    // editor
    ofxImGui::Gui gui;
    ImGuiDock::Dockspace dockspace;
    ImGuiDock::Dock rendererDock, cameraDock, modelDock,
                    sceneDock, previewDock, offlineResultDock,
                    materialDock, textureDock, 
                    shapeDock, lightDock;

    ofImage logo;
    GLuint logoButtonID;
    // version
    int versionMajor, versionMinor;

    // preview
    Graph3D* ground;
    float axisLength;
    a3EasyCam freeCam;
    a3MessageQueueIPC ipcC2S, ipcS2C;
    ofFbo sceneFbo, previewFbo, ipcFbo;
    ImVec2 previewContentRegion;
    // right-handed coordinate
    ofShader previewShader;
    // ray
    bool visualizeRays;
    // ray collections
    vector<a3Ray*> rayLists;

    // lights
    vector<a3EditorLightData*> lightList;

    // models
    vector<a3EditorModelData*> modelList;

    // materials
    vector<a3EditorMaterialData*> materialList;
    
    // textures
    vector<a3EditorTextureData*> textureList;

    // cameras
    vector<a3EditorCameraData*> cameraList;
    int activeCameraIndex;
    bool lockMouseInput;

    // shapes
    vector<a3EditorShapeData*> shapeList;

    // view mode
    // true freecam; false userdefined camera
    bool freeCamPreview;

    // renderer
    string saveImagePath;
    int gridLevel[2];
    bool enableGammaCorrection, enableToneMapping;
    int spp;
    int maxDepth, russianRouletteDepth;
    int integratorType, primitiveSetType;
};
