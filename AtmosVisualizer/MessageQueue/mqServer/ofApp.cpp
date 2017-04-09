#include "ofApp.h"

struct msgMouse :public a3MessageEntryHead
{
public:
    msgMouse() : x(0), y(0)
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
    mq.init(L"Whos Your Daddy", true, 100, 1024);

    ofSetWindowTitle("Server");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    char msg_buffer[1024];
    memset(msg_buffer, 0, 1024);

    a3MessageEntryHead* pMsg = (a3MessageEntryHead*) msg_buffer;

    // 处理消息 
    while(mq.dequeue(*pMsg))
    {
        switch(pMsg->type)
        {
        case 0:
        {
            const msgMouse* msg = (const msgMouse*) pMsg;
            //ofDrawCircle(ofPoint(msg->x, msg->y), 100);
            printf("server msg:%d %d\n", msg->x, msg->y);
            break;
        }

        case 1: 
        {
            const msgKeyboard* msg = (const msgKeyboard*) pMsg;
            printf("server msg:%c\n", msg->key);
            break;
        }
        }
    }
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
