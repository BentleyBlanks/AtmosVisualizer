#include "ofApp.h"

struct msgMouse :public a3MessageEntryHead
{
public:
    msgMouse(): x(0), y(0)
    {
        type = 0;
    }

    int x, y;
};

struct msgKeyboard :public a3MessageEntryHead
{
public:
    msgKeyboard()
    {
        type = 1;
    }

    int key;
};

//--------------------------------------------------------------
void ofApp::setup(){
    // 最大消息数量 单消息最大占字节
    mq.init(L"Whos Your Daddy", false, 100, 1024);

    ofSetWindowTitle("Client");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    //if(!mq.isFull())
    {
        msgKeyboard msg;
        msg.key = key;
        mq.enqueue(msg);
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    //if(!mq.isFull())
    {
        msgMouse msg;
        msg.x = x;
        msg.y = y;
        mq.enqueue(msg);
    }
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
