#pragma once

#include "ofMain.h"
#include "gwlistening.h"
#include "jukebox.h"


class ofApp : public ofBaseApp{

    //Jukebox to look after getting track lists and playing etc
    Jukebox juke;
    //Listening uses the ofSoundGetSpectrum() to get fft so only for ofSoundPlayers atm
    GwListening listen;
    //constants that affect the visualisation, both movement and shapes
    const static int connDist{30},clouds{50},cloudRadius{4},reps{7},twitchX{200},twitchY{200};
    constexpr static float ease{0.15}, reactionMagnifier{0.25};
    //reps is to do with the number of critters and their spacing (best with odd number for reps)
    int repsFloor = floor(reps/2.0);
    int repsCeil = floor((reps+1)/2);
    //undercoat is the bg colour and natural is the critter base colour
    ofColor  topReactiveColour, midReactiveColour, undercoat, natural,topColour;
    ofFbo canvas;
    float time0;
    float yTendency,nX,nY;
    float  tx[clouds], ty[clouds], motionSeeds[reps];
    ofPoint p[clouds];
public:

    void setup();
    void update();
    void draw();
    void seedCritters();
    //duration of the time-step (ie now-then)
    //reaction level [0..1] states how reactive they are
    //sizing is the result of calcSizeResponse()
    //x_ and y_reaction affects the speed and sway of the motion recc.[1..4]
    //if ease is small it will be twitchy, if larger, more saggy [0..1]
    void drawCritter(float duration, float reaction_level, float sizing, float x_reaction, float y_reaction, float ease_value);
    void fadeCanvas();
    ofColor updateColour();
    float calcSizeResponse();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

};
