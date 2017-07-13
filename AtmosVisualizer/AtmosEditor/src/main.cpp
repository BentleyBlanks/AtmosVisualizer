#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1680,900,OF_WINDOW);			// <-------- setup the GL context
    
    //ofGLWindowSettings settings;
    //settings.glVersionMajor = 3;
    //settings.glVersionMinor = 3;

    //settings.width = 1680;
    //settings.height = 900;
    //settings.windowMode = OF_WINDOW;
    //ofCreateWindow(settings);

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
