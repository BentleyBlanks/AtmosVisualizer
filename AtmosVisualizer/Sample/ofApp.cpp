#include "ofApp.h"
enum a3UniformDiskType
{
    CONCENTRIC,
    AREA
};

ofVec2f a3UniformSampleDisk(const float sampleU, const float sampleV, a3UniformDiskType type = CONCENTRIC)
{
    if(type == CONCENTRIC)
    {
        float phi, r, u, v;
        // (a,b) is now on [-1,1]ˆ2
        //float a = 2 * sampleU - 1;
        //float b = 2 * sampleV - 1;
        float a = sampleU;
        float b = sampleV;

        // region 1 or 2
        if(a > -b)
        {
            // region 1, also |a| > |b|
            if(a > b)
            {
                r = a;
                phi = (PI / 4) * (b / a);
            }
            // region 2, also |b| > |a|
            else
            {
                r = b;
                phi = (PI / 4) * (2 - (a / b));
            }
        }
        // region 3 or 4
        else
        {
            // region 3, also |a| >= |b|, a != 0
            if(a < b)
            {
                r = -a;
                phi = (PI / 4) * (4 + (b / a));
            }
            // region 4, |b| >= |a|, but a==0 and b==0 could occur.
            else
            {
                r = -b;
                if(b != 0)
                    phi = (PI / 4) * (6 - (a / b));
                else
                    phi = 0;
            }
        }

        u = r* cos(phi);
        v = r* sin(phi);
        return ofVec2f(u, v);
    }
    else if(type == AREA)
    {
        float r = sqrtf(sampleU);

        float theta = 2.0f * PI * sampleV;

        float u = r * cosf(theta);
        float v = r * sinf(theta);

        return ofVec2f(u, v);
    }
    else
    {
        // a3Log
        return ofVec2f::zero();
    }
}

enum SceneName
{
    UNIFORM_DISK,
    UNIFORM_CONCENTRIC_DISK
};

int sceneName = UNIFORM_DISK;

//--------------------------------------------------------------
void ofApp::setup(){
    gui.setup();

    drawLeft = true;
    drawRight = true;

    pointsNum = 100;
    for(int i = 0; i < pointsNum; i++)
    {
        rectPoints.push_back(ofVec2f(ofRandomf(), ofRandomf()));
    }

    for(auto p : rectPoints)
    {
        concentricPoints.push_back(a3UniformSampleDisk(p.x, p.y, CONCENTRIC));
        areaPoints.push_back(a3UniformSampleDisk(p.x, p.y, AREA));
    }
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    float width = ofGetWindowWidth();
    float height = ofGetWindowHeight();

    ofPushStyle();

    ofSetColor(0, 0, 0);
    float radiusW = width * 0.25, radiusH = height * 0.5;

    if(drawLeft)
    {
        for(auto p : rectPoints)
            ofDrawSphere(width * 0.25 + p.x * radiusW, height * 0.5 + p.y * radiusH, 1);
    }

    if(drawRight)
    {
        switch(sceneName)
        {
        case UNIFORM_CONCENTRIC_DISK:
            for(auto p : concentricPoints)
                ofDrawSphere(width * 0.75 + p.x * radiusW, height * 0.5 + p.y * radiusH, 1);
            break;

        case UNIFORM_DISK:
            for(auto p : areaPoints)
            {
                //printf("x:%f y:%f \n", p.x, p.y);
                ofDrawSphere(width * 0.75 + p.x * radiusW, height * 0.5 + p.y * radiusH, 1);
            }
            break;
        }
    }

    ofSetColor(255, 0, 0);
    ofNoFill();
    ofDrawRectangle(0, 0, width / 2.0f, height);
    ofDrawRectangle(width / 2.0f, 0, width / 2.0f, height);

    ofPopStyle();

    gui.begin();

    ImGui::Combo("Scene Name", &sceneName, "Area\0Concentric Disk");
    ImGui::DragFloat("Points Num", &pointsNum, 1, 100, 10000);

    if(ImGui::Button("Calculating"))
    {
        rectPoints.clear();
        concentricPoints.clear();
        areaPoints.clear();
        for(int i = 0; i < pointsNum; i++)
        {
            rectPoints.push_back(ofVec2f(ofRandomf(), ofRandomf()));
        }

        for(auto p : rectPoints)
        {
            concentricPoints.push_back(a3UniformSampleDisk(p.x, p.y, CONCENTRIC));
            areaPoints.push_back(a3UniformSampleDisk((p.x + 1.0f) * 0.5f, (p.y + 1.0f) * 0.5f, AREA));
        }
    }

    ImGui::Checkbox("Draw Left", &drawLeft);
    ImGui::Checkbox("Draw Right", &drawRight);

    gui.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
