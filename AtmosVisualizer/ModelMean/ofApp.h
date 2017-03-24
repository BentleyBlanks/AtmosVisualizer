#pragma once

#include "ofMain.h"

//#include "ofxAssimpModelLoader.h"
#include "ofxImGui.h"
//#include "Triangle.h"
#include "Shape.h"

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

    void loadModels();
    std::vector<ofVec3f>* updateMean(const std::vector<ofVec3f>* mean, const std::vector<ofVec3f>* ps2, float z, float weight);

    ofxImGui::Gui gui;


    std::vector<std::vector<ofVec3f>*> mean;
    std::vector<ofPolyline> meanFace;
    //ofPolyline meanPoly;
    // 一一对应
    //std::vector<ofVec3f> points;
    //std::vector<ofPolyline*> polylines;
    //std::vector<std::vector<ofVec3f>> points;

    //std::vector<std::vector<Triangle*>> shapes;
    //std::vector<ofxAssimpModelLoader> models;

    ofEasyCam* cam;
    //Triangle triangle;

    //std::vector<Shape*> shapes;

    bool drawPolylines, drawMean, drawFace;

    float z, zInterval;
    int rayCount;
    int modelNum;

    string prefix, saveFileName;
    //std::vector<ofVec3f> test;
    //std::vector<ofVec3f> rays;
};
