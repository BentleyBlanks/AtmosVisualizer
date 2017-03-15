#pragma once

#include "ofMain.h"
#include "a3Diffuse.h"
#include "a3OrenNayar.h"
#include "DrawUtils.h"
#include "ofxImGui.h"

enum BRDFType
{
    DIFFUSE = 0,
    OREN_NAYAR
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
    
    Graph3D* graph;
    a3Diffuse* diffuse;
    a3OrenNayar* orenNayar;

    ofVec3f incidentPosition;
    float brdfRadius;

    ofxImGui::Gui gui;
    const char* brdfNames;

    ofEasyCam cam;
    bool lockCam;
    float sphereRadius;

    int sampleNum;
    vector<ofVec3f> samplePoints;

    int brdfType;

    //ofVec3f R;
    //float orenNayarSigma;
};
