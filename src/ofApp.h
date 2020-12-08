#pragma once

#include "ofMain.h"
#include "gwlistening.h"

//#define bands 128
//#define connDist 30//join points with lines if they're this close
//just use the max rad instead
//#define maxPointOffset 1000
//#define peakDropRate 0.985
//#define bandBass 0
//#define bassSampleSize 2
//#define bandMidLow bandBass + bassSampleSize//new one for size
//#define midLowSampleSize 5
//#define bandMid bandMidLow + midLowSampleSize + 1
//((bands/3)*1)-30//trying to spot the vocal on billie eilish
//#define midSampleSize (bands - bandMid)/2
//#define bandTop 70
//#define topSampleSize 50
//#define clouds 50
//#define cloudRadius 4
//used for size of the critters (or clouds as they started)
//#define maxBassOut 400
//#define minBassOut 50
//used for the velocity of movement recc.[0.1..8]
//#define maxMidOut 2
//#define minMidOut 0.1
//#define maxTopOut 1
//#define minTopOut 0.5
//how many critters in the group (make it odd)
//#define reps 7
//#define repsFloor floor(reps/2)
//#define repsCeil int(floor((reps+1)/2))
//just to smooth movement a little in the ol'
//xy = ease*xy + (1-ease)*destXY stylee
//#define ease 0.15
//#define reactionMagnifier 2
//#define twitchX 100
//#define twitchY 120
//#define minTempoTime 2


class ofApp : public ofBaseApp{

    //Jukebox juke;
    const static int connDist{30},clouds{50},cloudRadius{4},reps{7},reactionMagnifier{2},twitchX{100},twitchY{120};
    constexpr static float ease{0.15};
    float repsFloor = floor(reps/2.0);
    float repsCeil = int(floor((reps+1)/2));
    GwListening listen;
    ofSoundPlayer player;
    //ofColor specBg, posBand, negBand;
    ofColor  topReactiveColour, midReactiveColour, undercoat, natural,topColour;
    ofFbo canvas;
    vector<ofFile> tList;
    ofDirectory tunes;
    float time0;
    //int specBarX, specBarY, specBarH, specBarW;
    //float specBarCol, colCorrect;
//    float tempoCount, tempo, tempoTolerance, tempoClock;
//    bool tempoSampling, tempoUp, tempoCaught;
//    float bass,midL, mid, top,bm,mlm,mm,tm;
//    float maxBassIn, minBassIn, maxMidIn, minMidIn;
//    float minTopIn, maxTopIn, minMidLIn, maxMidLIn;
    float destX, destY, yTendency,nX,nY;
//    float* spectrumIn;
    float /*spectrum[bands],*/ tx[clouds], ty[clouds], motionSeeds[reps];
    ofPoint p[clouds];
public:

    void setup();
    void update();
    void draw();
    void seedCritters();
//    void setupSpectrumGraph();
//    void updateSpectrum();
//    void drawSpectrum();
//    void updateRMS();
    void fadeCanvas();
    ofColor updateColour();
    float calcSizeResponse();
    void selectAndPlayTune();
    void clearTempo();
    void updateTempo();
    void clearTraining();

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
