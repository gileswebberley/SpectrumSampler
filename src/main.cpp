#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    //ofSetupOpenGL(2048,1024,OF_FULLSCREEN);			// <-------- setup the GL context
    ofGLFWWindowSettings settings;
        settings.windowMode = OF_FULLSCREEN;
        settings.multiMonitorFullScreen = true;

        ofCreateWindow(settings);
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ofApp());

}
