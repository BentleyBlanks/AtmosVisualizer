#include "a3Ray.h"

a3Ray::a3Ray(ofVec3f start, ofVec3f end)
    :start(start), end(end)
{
}

a3Ray::~a3Ray()
{
}

void a3Ray::draw()
{
    ofPushStyle();
    ofSetColor(ofColor(255, 255, 0));
    ofDrawLine(start, end);
    ofPopStyle();
}
