#include "ofApp.h"
#include "lineIntersect.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <algorithm>
#include <functional>
#include <vector>

const int modelNum = 4;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(50, 50, 50, 0);
    ofEnableDepthTest();

    // models
    loadModels();


    drawPolylines = true;
    drawMean = true;
    //loadModel("BJ037.obj");
    //loadModel("cd005.obj");
    //loadModel("cs015.obj");
    //loadModel("xa001.obj");

    cam = new ofEasyCam();

    gui.setup();

    rayCount = 360;
    zInterval = 5;
    //triangle.set(ofVec3f(10, 0, 0), ofVec3f(200, 500, 0), ofVec3f(-500, 0, 0), ofVec3f(200, -500, 0));
}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::loadModels()
{    
    for(int i = 1; i <= modelNum; i++)
    {
        string prefix = "face (";
        prefix += ofToString(i);
        prefix += ").obj";

        Shape* shape = new Shape();
        shape->load(prefix.c_str());

        shapes.push_back(shape);
    }
}

//--------------------------------------------------------------
std::vector<ofVec3f>* ofApp::updateMean(const std::vector<ofVec3f>* mean, const std::vector<ofVec3f>* ps2, float z)
{
    const int rayLength = 10000;
    std::vector<ofVec3f>* newMean = new std::vector<ofVec3f>();

    float angle = 0.0f;
    while(angle <= 360)
    {
        // 射线
        ofVec2f q1(0, 0), q2(cos(ofDegToRad(angle)) * rayLength, sin(ofDegToRad(angle)) * rayLength);

        ofVec2f result1, result2;

        bool intersect1 = false, intersect2 = false;

        for(int i = 0; i < mean->size()-1; i++)
        {
            ofVec2f p1((*mean)[i].x, (*mean)[i].y), p2((*mean)[i+1].x, (*mean)[i+1].y);
            if(segmentsIntersect(result1, p1, p2, q1, q2))
            {
                intersect1 = true;
                break;
            }
        }

        for(int i = 0; i < ps2->size()-1; i++)
        {
            ofVec2f p1((*ps2)[i].x, (*ps2)[i].y), p2((*ps2)[i + 1].x, (*ps2)[i + 1].y);
            if(segmentsIntersect(result2, p1, p2, q1, q2))
            {
                intersect2 = true;
                break;
            }
        }

        if(intersect1 && intersect2)
        {
            ofVec2f middle = (result1 + result2) / 2.0f;

            newMean->push_back(ofVec3f(middle.x, middle.y, z));
        }

        angle += 360.0f / rayCount;
    }

    return newMean;
}

//--------------------------------------------------------------
void ofApp::draw(){

    cam->begin();
    ofPushStyle();

    ofDrawAxis(400);

    ofSetColor(255, 255, 255);
    if(drawPolylines)
    {
        for(auto p : polylines)
            p->draw();
    }

    if(drawMean)
    {
        ofSetColor(255, 0, 0);
        meanPoly.draw();
    }

    if(drawFace)
    {
        ofSetColor(0, 255, 0);
        for(auto m : meanFace)
            m.draw();
    }

    //for(auto s : shapes)
    //    s->draw();

    ofPopStyle();
    cam->end();

    gui.begin();

    if(ImGui::DragFloat("Z", &z, 1, -100, 100))
    {
        polylines.clear();
        for(int i = 0; i < shapes.size(); i++)
        {
            polylines.push_back(shapes[i]->getPolylineWithZ(z));
        }
    }

    ImGui::DragFloat("z Interval", &zInterval, 1, 1, 50);
    ImGui::DragInt("Z", &rayCount, 720, 1, 3600);

    if(ImGui::Button("Update Polylines"))
    {
        polylines.clear();
        for(int i = 0; i < shapes.size(); i++)
        {
            polylines.push_back(shapes[i]->getPolylineWithZ(z));
        }
    }

    ImGui::Checkbox("Draw Polyline", &drawPolylines);

    ImGui::Checkbox("Draw Mean", &drawMean);

    ImGui::Checkbox("Draw Face", &drawFace);
    //for(int i = 0; i < shapes.size(); i++)
    //{
    //    string buttonName = "Update[";
    //    buttonName += ofToString(i);
    //    buttonName += "]";
    //
        //if(ImGui::Button(buttonName.c_str()))
        //{
        //    updateIntersection(i, z);
        //}
    //}

    if(ImGui::Button("Calculate"))
    {
        //polylines.clear();
        //for(int i = 0; i < shapes.size(); i++)
        //{
        //    polylines.push_back(shapes[i]->getPolylineWithZ(z));
        //}

        if(shapes.size() > 0)
        {
            // 第一个形状上的交点
            std::vector<ofVec3f>* tempPoints = shapes[0]->getPointsWithZ(z);
            for(int i = 1; i < shapes.size(); i++)
            {
                std::vector<ofVec3f>* temp = NULL;
                temp = updateMean(tempPoints, shapes[i]->getPointsWithZ(z), z);


                // 释放上一次结果
                delete tempPoints;
                tempPoints = temp;
            }

            if(!tempPoints->empty())
            {
                meanPoly.clear();
                meanPoly.addVertices(*tempPoints);
            }

            delete tempPoints;
            tempPoints = NULL;
        }
    }

    if(ImGui::Button("Calculate Face"))
    {
        if(shapes.size() > 0)
        {
            float zStart = -100;
            while(zStart <= 100)
            {
                ofPolyline poly;
                // 第一个形状上的交点
                std::vector<ofVec3f>* tempPoints = shapes[0]->getPointsWithZ(zStart);
                for(int i = 1; i < shapes.size(); i++)
                {
                    std::vector<ofVec3f>* temp = NULL;
                    temp = updateMean(tempPoints, shapes[i]->getPointsWithZ(zStart), zStart);


                    // 释放上一次结果
                    delete tempPoints;
                    tempPoints = temp;
                }

                if(!tempPoints->empty())
                {
                    poly.clear();
                    poly.addVertices(*tempPoints);
                    meanFace.push_back(poly);
                }

                delete tempPoints;
                tempPoints = NULL;

                zStart += zInterval;
            }
        }
    }

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
