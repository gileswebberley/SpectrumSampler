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
    const static int connDist{30},clouds{50},cloudRadius{4},reps{7},reactionMagnifier{2},twitchX{100},twitchY{120};
    constexpr static float ease{0.15};
    //reps is to do with the number of critters and their spacing (best with odd number for reps)
    float repsFloor = floor(reps/2.0);
    float repsCeil = int(floor((reps+1)/2));
    //undercoat is the bg colour and natural is the critter base colour
    ofColor  topReactiveColour, midReactiveColour, undercoat, natural,topColour;
    ofFbo canvas;
    float time0;
    float destX, destY, yTendency,nX,nY;
    float  tx[clouds], ty[clouds], motionSeeds[reps];
    ofPoint p[clouds];
public:

    void setup();
    void update();
    void draw();
    void seedCritters();
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
