#include "ofApp.h"
ofVec3f a3Hemisphere(float u1, float u2)
{
    const float r = sqrt(u1);
    const float theta = 2 * PI * u2;

    const float x = r * cos(theta);
    const float y = r * sin(theta);

    return ofVec3f(x, sqrt(max(0.0f, 1 - u1)), y);
}

//--------------------------------------------------------------
void ofApp::setup(){
    brdfNames = "Diffuse\0Oren-Nayar";
    //brdfNames.push_back("Oren-Nayar\0");
    sphereRadius = 1.2;

    brdfRadius = 800;

    lockCam = false;

    gui.setup();

    diffuse = new a3Diffuse();
    orenNayar = new a3OrenNayar();
    orenNayar->sigma = 20;

    graph = new Graph3D(1000, 1000, 35, 35);

    incidentPosition.set(200, 200, 200);

    sampleNum = 100;
    for(int i = 0; i < sampleNum; i++)
    {
        samplePoints.push_back(a3Hemisphere(ofRandomuf(), ofRandomuf()));
    }
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    gui.begin();

    ImGui::SliderFloat3("Incident Position", &incidentPosition.x, -500.0f, 500.0f);

    if(ImGui::Checkbox("lock camera", &lockCam))
    {
        if(lockCam)
            cam.disableMouseInput();
        else
            cam.enableMouseInput();
    }

    ImGui::SliderFloat("Point Radius", &sphereRadius, 0.5f, 10.0f);

    if(ImGui::SliderInt("Sample Point Number", &sampleNum, 10, 10000))
    {
        samplePoints.clear();
        for(int i = 0; i < sampleNum; i++)
        {
            samplePoints.push_back(a3Hemisphere(ofRandomuf(), ofRandomuf()));
        }
    }

    ImGui::Combo("BRDF Types", &brdfType, brdfNames);

    ImGui::SliderFloat("BRDF Radius", &brdfRadius, 10.0f, 2500.0f);

    float R;
    switch(brdfType)
    {
    case DIFFUSE:
        R = diffuse->R.x;
        if(ImGui::SliderFloat("R", &R, 0.0f, 1.0f))
        {
            diffuse->R.set(R);
        }
        break;
    case OREN_NAYAR:
        R = diffuse->R.x;
        if(ImGui::SliderFloat("R", &R, 0.0f, 1.0f))
        {
            orenNayar->R.set(R);
        }
        ImGui::SliderFloat("Sigma Degree", &orenNayar->sigma, 0.0f, 180.0f);
        break;
    }

    gui.end();

    cam.begin();

    ofPushStyle();

    ofSetColor(ofColor(70, 70, 70, 50));
    graph->draw();
    ofDrawAxis(250);

    ofPopStyle();

    // »Î…‰π‚
    ofPushStyle();

    // incident radiance
    ofSetLineWidth(5);
    ofSetColor(ofColor(255, 0, 0));
    ofDrawArrow(ofVec3f(), incidentPosition, 5);

    ofSetColor(ofColor(20, 20, 20));

    // brdf
    for(int i = 0; i < samplePoints.size(); i++)
    {
        ofVec3f brdf;
        switch(brdfType)
        {
        case DIFFUSE:
            brdf = diffuse->evaluate(incidentPosition.getNormalized(), samplePoints[i]);
            break;
        case OREN_NAYAR:
            brdf = orenNayar->evaluate(incidentPosition.getNormalized(), samplePoints[i]);
            break;
        }
        ofDrawSphere(brdf * samplePoints[i] * brdfRadius, sphereRadius);
    }

    ofPopStyle();

    cam.end();
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
