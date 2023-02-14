#ifndef GWLISTENING_H
#define GWLISTENING_H
#include "ofMain.h"

class GwListening
{
    //bands - number of sample buckets
    //bandBass - the lowest bucket to 'listen to' (if you find the lowest buckets are twisting the listening then use this)
    //bassSampleSize - number of buckets that count toward the bass 'listening' [bandBass..bassSampleSize+bandBass]
    int bands{256},bandBass{0},bassSampleSize{3};
    //bandMidLow - index of the start bucket of mid-low, which is one above the top of the bass range, listening range [bandMidLow..midLowSampleSize+bandMidLow]
    int bandMidLow{bandBass+bassSampleSize+1},midLowSampleSize{15};
    //bandMid - index of start bucket of mid, one above the end of mid-low, listening range
    //midSampleSize is half of what's left in terms of buckets
    int bandMid{bandMidLow+midLowSampleSize+1},midSampleSize{((bands-1)-bandMid)/2};
    //bandTop - index of start bucket of top, one above the end of mid, listening range
    //topSampleSize is all of the buckets left
    int bandTop{bandMid+midSampleSize},topSampleSize{(bands-1)-bandTop};
    //for the spectrum graph visualisation
    int specBarX, specBarY, specBarH, specBarW;
    //for fitting the columns (that refer to buckets of the sample) according to screen width
    float specBarCol, colCorrect;
    ofColor specBg{46}, posBand{156}, negBand{128}, topColour;
    //literally the rate at which the columns fade back down from peaks
    float peakDropRate{ 0.985 };

    //minTempoTime - to restrict catching on to a tempo too early
    //tempoBeatsInBar - to set the time signature that it's expecting (4/4)
    float minTempoTime{2}, tempoBeatsInBar{4};
    float tempoCount{0}, tempo{8.0}, tempoTolerance, tempoClock{0};
    bool tempoSampling, tempoUp, tempoCaught;
    //these are mapped rms from max-min-in to max-min-out values
    float bass,midL, mid, top;
    //these are the raw rms figures for each band-group
    float bm,mlm,mm,tm;
    //these are for mapping, because readings of top frequencies from fft are such a huge range that they average out as tiny readings
    //max values are updated if the value goes above what's expected 
    //the 'In' values are what we are getting from the raw fft spectrum sampler - see clearTraining() for values
    float maxBassIn, minBassIn, maxMidIn, minMidIn;
    float minTopIn, maxTopIn, minMidLIn, maxMidLIn;
    //these are the scaling factors that the 'In' values are mapped to
    float maxBassOut{500},minBassOut{10},maxMidLOut{300},minMidLOut{10};
    float maxMidOut{300},minMidOut{60}, maxTopOut{200}, minTopOut{10};
    //this is the pointer returned by ofSoundGetSpectrum() - in the guide it says not to delete this pointer (I guess it is taken care of in ofSound?)
    float* spectrumIn;
    //this is the spectrum that has the peak fading applied
    std::vector<float> spectrum;
    //an attempt to make a more natural representation of the fft sample
    std::vector<float> levelisedSpectrum;

    void updateRMS();

public:
    GwListening();
    //set up the default values as above
    //void setToDefault(); this was just making the same stuff be set in more than one place - not good
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
    //returns the minOut and maxOut for frange
    std::pair<float,float> getOutPair(char frange);
    //returns the minIn and maxIn for frange
    std::pair<float,float> getInPair(char frange);
    float normFromBass(){return ofNormalize(bm,minBassIn,maxBassIn);}
    float normFromMidLow(){return ofNormalize(mlm,minMidLIn,maxMidLIn);}
    float normFromMidHigh(){return ofNormalize(mm,minMidIn,maxMidIn);}
    float normFromTop(){return ofNormalize(tm,minTopIn,maxTopIn);}
    //returns [0..1] as to where s is in the range given eg [minBassOut..maxBassOut]
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
    bool isTempoSampled(){return tempoCaught;}
    void clearTraining();
};

#endif // GWLISTENING_H
