#include "ofApp.h"
/*
 * 03-05-2020 it's reacting beautifully to bad guy and has nicely
 * centralised settings but needs a bloody good clean up and probs
 * a bit of a class thing going on, there seems to be far too many
 * groups of related variables so it's begging for it.
 * ooh it works with roy's tune too
*/
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    canvas.allocate(ofGetWidth(),ofGetHeight(),GL_RGBA);
    time0 = 0;
//    clearTempo();
//    setupSpectrumGraph();
    seedCritters();
    //let's set the measurment to 70% peak
//    tempoTolerance = maxBassOut*0.65;
    //this is the "training" area
    midReactiveColour = ofColor(246,76,14).invert()/2;
    topReactiveColour = ofColor(14,155,246).invert()/2;
    //undercoat.a controls the fade rate of trails
    undercoat = ofColor(70,90,70,50);
    natural = ofColor(255);
    listen.clearTraining();
    //right let's make it pick a song from the data folder
    //ofDirectory tunes;
    tunes.open("./");
    tunes.allowExt("wav");
    tunes.allowExt("mp3");
    tunes.listDir();
    tList = tunes.getFiles();
    //selectAndPlayTune();
}

//void ofApp::clearTraining(){
//    maxBassIn = 0.8;
//    minBassIn  = 0.01;
//    maxMidLIn = 0.5;
//    minMidLIn = 0.001;
//    maxMidIn = 0.1;
//    minMidIn = 0.01;
//    maxTopIn = 0.005;
//    minTopIn = 0.000001;
//}

void ofApp::seedCritters(){
    //I'm going to make a pointer to seed the random
    //makes it far more random each time
    //it's run with the use of the pointer value :)
    long tmp;
    long* tmpPnt = &tmp;
    cout<<"temp point "<<(long)tmpPnt<<"\n";/*
    for(int i =0; i<bands;i++){
        spectrum[i] = 0.0f;
    }*/
    std::pair<float,float> bassOut = listen.getOutPair('b');//returns <minBassOut,maxBassOut>
    cout<<"the pair of values are:"<<bassOut.first<<"\t:"<<bassOut.second<<"\n";
    for(int i =0;i<clouds; i++){
        int seed = ofGetElapsedTimeMicros()+i+(long)tmpPnt;
        ofSeedRandom(seed);
        //so use gwlistening.setBassOut() with these
        tx[i] = ofRandom(bassOut.first,bassOut.second);
        ty[i] = ofRandom(bassOut.first,bassOut.second);
    }
    //set a random frenzy value for each critter
    for(int i = 0; i < reps; i++){
        int seed = ofGetElapsedTimeMicros()+i*(long)tmpPnt;
        ofSeedRandom(seed);
        //equiv to ofRandom(-1.0,1.0);
        motionSeeds[i] = ofRandomf();
        cout<<"motion seed"<<i<<": "<<motionSeeds[i]<<"\n";
    }
    //I want the y movement to be affected by the music more
    yTendency = 0;
}

//void ofApp::setupSpectrumGraph(){
//    for(int i =0; i<bands;i++){
//        spectrum[i] = 0.0f;
//    }
//    //set up variables for the spectrum graph
//    //spectrum chart background colour is 18% grey
//    specBg = ofColor(46);
//    posBand = ofColor(156);
//    negBand = ofColor(128);
//    specBarX = 10;
//    specBarW = ofGetWidth() - 20;
//    specBarH = 300;
//    specBarY = ofGetHeight();
//    //why don't the frequency bars not spread across the screen??
//    if(bands == 128){
//        colCorrect = 0.85;
//    }else if(bands == 256){
//        colCorrect = 0.92;
//    }//but where do these numbers come from???
//    specBarCol = round(specBarW/(bands))+colCorrect;
//}

void ofApp::selectAndPlayTune(){
    int noOfTunes = tList.size();
    //create ptr for random seed
    int* tmpPnt = &noOfTunes;
    //cout << "number of tunes counted: "<<noOfTunes<<"\n";
    int seed = ofGetElapsedTimeMicros()+(long)tmpPnt;
    ofSeedRandom(seed);
    int selectTune = floor(ofRandom(1.0)*(noOfTunes-1));
    //cout << "tune selected: "<<selectTune<<"\n";
    //tunes.sort();
    //string tune = tunes.getPath(selectTune);
    //player.load(tune);
    player.load(tList[selectTune]);
    tList.erase(tList.begin()+selectTune);
    tList.shrink_to_fit();
    cout<<"tList size is: "<<tList.size()<<"\n";
    if(tList.empty()){
        tList = tunes.getFiles();
    }
    player.setLoop(false);
    player.play();
    //player.setVolume(0.9);
}

//void ofApp::clearTempo(){

//    tempoSampling = false;
//    tempoCaught = false;
//    tempoUp = false;
//    //maybe the first 4 bass hits (incase of dbl hits)
//    tempoCount = 0;
//    //set the length of tempo in seconds
//    tempo = 30.0;
//    tempoClock = ofGetElapsedTimef();
//}

//void ofApp::updateTempo(){
//    if(bass >= tempoTolerance && tempoSampling == false && tempoCaught == false){
//        tempoClock = ofGetElapsedTimef();
//        tempoSampling = true;
//        tempoUp =true;
//        tempoTolerance = bass;
//        cout<<"in the first tempo sample, bass up\n";
//    }
//    if(!tempoCaught && tempoSampling){
//        if(bass < tempoTolerance && tempoUp == true){
//            tempoUp = false;
//            tempoCount++;
//            cout<<"bass down\n";
//        }
//        if(tempoCount < 4){

//            if(bass >= tempoTolerance && tempoUp == false){
//                tempoUp = true;
//                cout<<"bass up\n";
//            }
//        }else{
//            float now = ofGetElapsedTimef();
//            tempo = now - tempoClock;
//            if(tempo > minTempoTime){
//            tempoCaught = true;
//            tempoSampling = false;
//            }else{
//                //dbl sample time
//                tempoCount = 0;
//            }
//            cout<<"length of tempo measured is: "<<tempo<<"\n";
//        }
//}
//}

//void ofApp::updateSpectrum(){
//    //pointer passed by our player to real spectrum
//    //do not release from memory!!
//    spectrumIn = ofSoundGetSpectrum(bands);
//    //fix positioning and bar chart when maximised
//    specBarY = ofGetHeight()-20;
//    specBarW = ofGetWidth()-20;
//    specBarCol = ceil(specBarW/(bands))+colCorrect;
//    //make the values decrease slowly until bumped back up
//    for(int i = 0; i<bands; i++){
//        spectrum[i] *= ofNoise(peakDropRate);
//        spectrum[i] = max(spectrum[i], spectrumIn[i]);
//    }
//}

//void ofApp::drawSpectrum(){
//    //draw the spectrum chart
//    //ofSetColor(specBg);
//    //ofFill();
//    //ofDrawRectangle(specBarX, specBarY, specBarW,-specBarH/2);
//     //ofSetColor(posBand);
//    topColour.a = 128;
//     for(int i = 0; i < bands; i++){
//         if(i==bandBass ||i==bandMidLow || i==bandMid || i==bandTop){
//             ofSetColor(topColour);
//         }else{
//             ofSetColor(topColour);
//         }
//         //try to level the top freq up
//         float compV = spectrum[i];
//         float numerator = i;
//         float denominator = bands/10;
//         float multiplier = 1.1;//i/20;
//         if(i>19){
//             compV = ofClamp(compV*(numerator/denominator),0,2)*multiplier;
//         }else if(i<2){
//             compV /= 2;
//         }
//         //ofFill();
//         ofSetCircleResolution(30);
//         ofDrawCircle(specBarX+i*specBarCol,specBarY+10,compV*specBarH/2);
//         //ofDrawRectangle(specBarX+i*specBarCol,specBarY,specBarCol-2,(-10)-compV*specBarH);
//         //ofNoFill();
//     }
//}

//void ofApp::updateRMS(){
//    //produce rms type figures for each band sample
//    bm = 0;
//    for(int i=0; i<bassSampleSize; i++){
//        bm += pow(spectrum[bandBass+i],2.0);
//    }
//    bm /= bassSampleSize;
//    bm = sqrt(bm);
//    mlm = 0;
//    for(int i=0; i<midLowSampleSize; i++){
//        mlm += pow(spectrum[bandMidLow+i],2.0);
//    }
//    mlm /= midLowSampleSize;
//    mlm = sqrt(mlm);
//    mm = 0;
//    for(int i=0; i<midSampleSize; i++){
//        mm += pow(spectrum[bandMid+i],2.0);
//    }
//    mm /= midSampleSize;
//    mm = sqrt(mm);
//    tm = 0;
//    for(int i=0; i<topSampleSize; i++){
//        tm += pow(spectrum[bandTop+i],2.0);
//    }
//    tm /= topSampleSize;
//    tm = sqrt(tm);
//    //make sure that the ranges don't get zeroed out
//    //whilst trying to 'learn' the song's ranges
//    if(bm != 0.0){
//        minBassIn = min(bm,minBassIn);
//    }
//    maxBassIn = max(bm,maxBassIn);
//    if(mlm != 0.0){
//        minMidLIn = min(mlm,minMidLIn);
//    }
//    maxMidLIn = max(mlm,maxMidLIn);
//    if(mm != 0.0){
//        minMidIn = min(mm,minMidIn);
//    }
//    maxMidIn = max(mm,maxMidIn);
//    if(tm != 0){
//        minTopIn = min(tm,minTopIn);
//    }
//    maxTopIn = max(tm, maxTopIn);

//    //map the readings to the specified ranges
//    bass = ofMap(bm,minBassIn,maxBassIn,minBassOut,maxBassOut,true);
//    midL = ofMap(mlm,minMidLIn,maxMidLIn,minBassOut,maxBassOut,true);
//    mid = ofMap(mm,minMidIn,maxMidIn,minMidOut,maxMidOut, true);
//    top = ofMap(tm,minTopIn,maxTopIn,minTopOut,maxTopOut);
//}

ofColor ofApp::updateColour(){
    //work out the colour based on top and mid ranges (red:mid blue:top)
    //just builds up to max colour then doesn't go back down with lerp()
    //normalize is the same as map to [0..1]
    float cShiftr = (listen.normFromMidLow()*2)/3;
    cShiftr += listen.normFromBass()/3;
    float cShiftb = (listen.normFromTop()/2)+(listen.normFromMidHigh(listen.normFromMidLow())/2);
    ofColor tmpColour(midReactiveColour*cShiftr);
    tmpColour += ofColor(topReactiveColour*cShiftb);
    return tmpColour;
}

void ofApp::fadeCanvas(){
    //fade for a little smoothing trail
    ofFill();
    //make the alpha of undercoat smaller to make trail longer
    ofSetColor(undercoat);
    ofDrawRectangle(0,0,canvas.getWidth(),canvas.getHeight());
    ofNoFill();
}

float ofApp::calcSizeResponse(){
    //hopefully getting the data as pairs it makes this work again
    std::pair<float,float> midInPair = listen.getInPair('m');
    std::pair<float,float> bassOutPair = listen.getOutPair('b');
    std::pair<float,float> topOutPair = listen.getOutPair('t');
    //smaller than mid if toppy, bigger if bassy
    //float cleanMid = listen.normFromBassOut(listen.normFromMidHigh(listen.getMidHigh()));
    float cleanMid = ofMap(listen.getMidHigh(),midInPair.first,midInPair.second,bassOutPair.first,bassOutPair.second,true);
    //with gwListener bassMid = normFromBassOut(getMidLow())*cleanMid
    //float bassMid = listen.normFromBassOut(listen.getMidLow())*cleanMid;
    float bassMid = (ofNormalize(listen.getMidLow(),bassOutPair.first,bassOutPair.second))*cleanMid;//(ofMap(mid,minMidIn,maxMidIn,minBassOut,maxBassOut*2,true));//-midL;
    //with gwListener topmid = (1-normFromTopOut(getTop()))*cleanMid
    //float topMid = (1-listen.normFromTopOut(listen.getTop()))*cleanMid;
    float topMid = (1-(ofNormalize(listen.getTop(),topOutPair.first,topOutPair.second)))*cleanMid;
    //float combiMid = max(bassMid,cleanMid);
    float combiMid = min(cleanMid,topMid);
    combiMid += bassMid/1.2;
    return combiMid;
}

//--------------------------------------------------------------
void ofApp::update(){
    float t = ofGetElapsedTimef();
    float dt = t-time0;
    dt = ofClamp(dt,0.0,0.1);
    time0 = t;
    if(player.getPosition() == 0){
        cout<<"I've finished the song!!\n";
        seedCritters();
        listen.clearTraining();
        selectAndPlayTune();
        listen.clearTempo();
    }
    ofSoundUpdate();
    listen.updateSpectrum();
    //updateRMS();
    listen.updateTempo();
    topColour = updateColour();
    topColour = natural-topColour;
    //drawing onto an ofFbo
    canvas.begin();
    fadeCanvas();
    listen.drawSpectrum();
    float repWidth = ofGetWidth()/reps;
    for(int rep = 1; rep <= reps; rep++){
        ofPushMatrix();
        float tT = ofGetElapsedTimef();
        bool motionDir = (motionSeeds[rep-1]<0)?false:true;
        //absolute value for bass, don't want them floating up on the drop!?
        float mappedMotionAbs = ofMap(abs(motionSeeds[rep-1]),0,1,0.7,1);
        float mappedMotion = mappedMotionAbs;
        if(!motionDir){
            mappedMotion *= -1;
        }

        nX = ((repWidth)*rep)-(repWidth/2);
        if(rep<= repsFloor){
            //work out x-coord and add 'personality' to their individual
            //movement based on the tempo
            nX += (twitchX*mappedMotion*fmodf(ofSignedNoise(tT),listen.getTempo()/2));
            topColour.a = 100+rep*(155/repsCeil);
        }else if(rep > repsCeil){
            //movement based on tempo but inverted from other side of centre
            nX -= (twitchX*mappedMotion*fmodf(ofSignedNoise(tT),listen.getTempo()/2));
            topColour.a = 255-(fmodf(rep,repsCeil)*(155/(repsCeil)));
        }else if(rep == repsCeil){
            //no sideways movement for the centre critter
            nX -= ((1.5*twitchX)*mappedMotion*fmodf(ofSignedNoise(tT),listen.getTempo()/2));
            topColour.a = 255;
        }
        //make the group head down with bass hit and up with top snaps
        //great, works pretty nicely first time
        yTendency = yTendency*listen.getPeakDrop();
        //so ease will be stickier if bigger
        float bassDrag =(ease*yTendency);
        bassDrag += (1-ease)*(ofMap(listen.normFromBass(),0.575,1,0,1,true));
        float topLift = ((1-ease)*yTendency)+(ease*ofMap(listen.normFromTop(),0.4,0.9,0,1));
        float bDrag = (bassDrag*(listen.normFromBass()))-topLift;
        yTendency = max(yTendency,bDrag);

        float halfHt =ofGetHeight()/2;
        nY = halfHt-(halfHt/3)+(((halfHt/4)*mappedMotion)*fmodf(ofSignedNoise(tT),listen.getTempo()))+(((halfHt)*mappedMotionAbs)*yTendency);
        ofTranslate(nX,nY);
        ofRotateDeg(360/rep);
        ofSetColor(topColour);
        ofFill();
        //make a polygon with curved edges to make it more 'organic'
        //so set the first point as an anchor
        ofPolyline critter;
        critter.addVertex(p[0]);
        float reactionLevel = mappedMotion*reactionMagnifier;
        float combiMid = calcSizeResponse();
        for(int i = 0; i < clouds; i++){
            tx[i] += ((reactionLevel)*dt)*(listen.getMidHigh()+listen.normFromTopOut(listen.getTop())+listen.normFromBassOut(listen.getMidLow()));
            ty[i] += ((reactionLevel)*dt)*(listen.getMidHigh()+listen.normFromTopOut(listen.getTop())+listen.normFromBassOut(listen.getMidLow()));

            //change from bass to midL for size
            destX = ofSignedNoise(tx[i])*(reactionLevel*(combiMid));//+bass));
            destY = ofSignedNoise(ty[i])*(reactionLevel*(combiMid));//+bass));
            p[i].x = (ease*p[i].x) +(1-ease)*(destX);
            p[i].y = (ease*p[i].y) +(1-ease)*(destY);

            critter.addVertex(p[i]);
            for(int j = i+1; j < clouds; j++){
                //WAY more processing!!
                critter.curveTo(p[j],10);
            }
            ofDrawCircle(p[i],cloudRadius);
        }
        critter.addVertex(p[clouds-1]);
        ofSetLineWidth(cloudRadius/2);
        critter.draw();
        ofPopMatrix();
    }
    canvas.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(70,90,70,0);
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    canvas.draw(0,0);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'b'){
        //doesn't work??
        player.setSpeed(-3.0);
    }
    if(key == 'm'){
        player.setSpeed(3.0);
    }
    if(key == ' '){
        if(player.isPlaying() && listen.isTempoSampling()){
            listen.clearTempo();
        }
        player.setPaused(player.isPlaying());
    }
    if(key == OF_KEY_RETURN){
        cout<<"skip tune......"<<player.getPosition()<<"\n";
        player.stop();
        player.setPosition(1.0);
        cout<<"position after: "<<player.getPosition()<<"\n";
        //selectAndPlayTune();
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == 'b' || key == 'm'){
        if(listen.isTempoSampling()){
            listen.clearTempo();
        }
        player.setSpeed(1);
    }
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
