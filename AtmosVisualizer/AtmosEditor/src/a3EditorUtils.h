#pragma once
#include "ofMain.h"
#include <codecvt>

// ×óÊÖ×ø±êÏµ
void a3DrawAxis(float axisLength)
{
    // axis
    ofPushStyle();

    ofVec3f x(axisLength, 0, 0), y(0, -axisLength, 0), z(0, 0, axisLength);

    ofSetColor(ofColor::red);
    ofLine(ofPoint(), x);

    ofSetColor(ofColor::green);
    ofLine(ofPoint(), y);

    ofSetColor(ofColor::blue);
    ofLine(ofPoint(), z);

    ofPopStyle();

    // axis name
    ofDrawBitmapString("x", x);
    ofDrawBitmapString("y", y);
    ofDrawBitmapString("z", z);
}

inline wstring a3S2WS(const std::string& str)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

inline string a3WS2S(const std::wstring& wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}
