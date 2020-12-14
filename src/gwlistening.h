#ifndef GWLISTENING_H
#define GWLISTENING_H
#include "ofMain.h"

class GwListening
{
    //not sure so I'm not going to make them constants
    int bands{256},bandBass{0},bassSampleSize{2};
    int bandMidLow{bandBass+bassSampleSize},midLowSampleSize{5};
    int bandMid{bandMidLow+midLowSampleSize+1},midSampleSize{((bands-1)-bandMid)/2};
    int bandTop{bandMid+midSampleSize},topSampleSize{(bands-1)-bandTop};
    int specBarX, specBarY, specBarH, specBarW;
    float specBarCol, colCorrect;
    ofColor specBg{46}, posBand{156}, negBand{128}, topColour;
    int minTempoTime{2};
    float tempoCount, tempo, tempoTolerance, tempoClock;
    bool tempoSampling, tempoUp, tempoCaught;
    //these are mapped rms from max-min-in to max-min-out values
    float bass,midL, mid, top;
    //these are the raw rms figures for each band-group
    float bm,mlm,mm,tm;
    float peakDropRate{0.985};
    float maxBassIn, minBassIn, maxMidIn, minMidIn;
    float minTopIn, maxTopIn, minMidLIn, maxMidLIn;
    //these are the scaling factors that are mapped to
    float maxBassOut{400},minBassOut{50},maxMidLOut{2},minMidLOut{0.1};
    float maxMidOut{2},minMidOut{0.1}, maxTopOut{100}, minTopOut{0.5};
    std::vector<float> spectrum;
    std::vector<float> levelisedSpectrum;
    void updateRMS();

public:
    GwListening();
    //set up the default values as above
    void setToDefault();
    void setupSpectrumGraph(int x, int y, int w, int h);
    const std::vector<float>* getLevelisedSpectrum();
    void setBassOut(float min, float max){minBassOut = abs(min); maxBassOut = abs(max);}
    void setMidOut(float min, float max){minMidOut = abs(min); maxMidOut = abs(max);}
    void setTopOut(float min, float max){minTopOut = abs(min); maxTopOut = abs(max);}
    //the return values should be read only, I believe the const here does that?
    float getBass()const{return bass;}
    float getMidLow()const{return midL;}
    float getMidHigh()const{return mid;}
    float getTop()const{return top;}
    float getTempo()const{return tempo;}
    float getPeakDrop()const{return peakDropRate;}
    //frange [b,l,m,t]
    std::pair<float,float> getOutPair(char frange);
    std::pair<float,float> getInPair(char frange);
    float normFromBass(){return ofNormalize(bm,minBassIn,maxBassIn);}
    float normFromMidLow(){return ofNormalize(mlm,minMidLIn,maxMidLIn);}
    float normFromMidHigh(){return ofNormalize(mm,minMidIn,maxMidIn);}
    float normFromTop(){return ofNormalize(tm,minTopIn,maxTopIn);}
    //s[0..1] so scale to the range given eg [minBassOut..maxBassOut]
    float normFromBass(float s){return ofNormalize(s,minBassIn,maxBassIn);}
    float normFromBassOut(float s){return ofNormalize(s,minBassOut,maxBassOut);}
    float normFromMidLow(float s){return ofNormalize(s,minMidLIn,maxMidLIn);}
    float normFromMidHigh(float s){return ofNormalize(s,minMidIn,maxMidIn);}
    float normFromTop(float s){return ofNormalize(s,minTopIn,maxTopIn);}
    float normFromTopOut(float s){return ofNormalize(s,minTopOut,maxTopOut);}
    void updateSpectrum();
    void drawSpectrum();
    void clearTempo();
    void updateTempo();
    bool isTempoSampling(){return tempoSampling;}
    void clearTraining();
};

#endif // GWLISTENING_H
