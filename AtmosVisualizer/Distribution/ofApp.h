#pragma once

#include "ofMain.h"
#include "a3Distribution1D.h"
#include "a3Distribution2D.h"
#include "a3Coordinate.h"
#include "a3SamplingImage.h"
#include "ofxImGui.h"
#include "DrawUtils.h"

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

    void guiControl();

    void setupSampling1D();
    void setupSampling2D();
    void setupSamplingHemisphere();
    void setupSamplingSphere();
    void setupSamplingCone();

    void drawSampling1D();
    void drawSampling2D();
    void drawSamplingHemisphere();
    void drawSamplingUniformHemisphere();
    void drawSamplingSphere();
    void drawSamplingCone();

    // 1D
    a3Distribution1D *distribution1D;
    vector<float> *pdf, sampled;
    ofBitmapFont left, right;
    int numOfSampled1D, numOfData;
    a3Coordinate coordinateLeft, coordinateRight, coordinateSample;

    // 2D
    a3Distribution2D *distribution2D;
    vector<float> imageData;
    int numOfSampled2D, dataWidth, dataHeight;
    vector<ofVec2f> points;
    a3SamplingImage* samplingImage;
    ofImage image;
    ofFloatImage hdrImage;
    bool drawOriginal, drawSample, draw3D;

    ofFbo fbo;
    ofxImGui::Gui gui;
    int sceneName;

    // 3D
    ofEasyCam cam;
    Graph3D *graph;

    // Hemisphere
    vector<ofVec3f> hemiPoints1, hemiPoints2, hemiUniPoints, spherePoints;
    int hemiPointNum, spherePointNum;
    int hemiSampleType;
    float hemiRadius, sphereRadius;

    // Cone
    vector<ofVec3f> conePoints;
    int conePointNum;
    float coneRadius, cosThetaMax;
};
