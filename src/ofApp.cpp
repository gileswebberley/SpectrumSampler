#include "ofApp.h"
/*
 * 03-05-2020 it's reacting beautifully to bad guy and has nicely
 * centralised settings but needs a bloody good clean up and probs
 * a bit of a class thing going on, there seems to be far too many
 * groups of related variables so it's begging for it.
 * ooh it works with roy's tune too
 *
 * 12-12-2020
 * so I got up and running with github which gave me the confidence
 * to rip this apart and put back together with a better structure
 * made up of a new Listener and Jukebox cos it was all twisted into
 * one god-class as it was the first thing with OF
*/
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofHideCursor();
    //allocate the canvas for drawing onto
    canvas.allocate(ofGetWidth(),ofGetHeight(),GL_RGBA);
    time0 = 0;
    //give each critter slightly randomised behaviour
    seedCritters();
    //colour tendancy by bass and mid-low
    midReactiveColour = ofColor(204,48,0).invert();///2;
    //colour tendancy by mid-high and top
    topReactiveColour = ofColor(99,255,203).invert();///2;
    //undercoat.a controls the fade rate of trails
    undercoat = ofColor(70,90,70,50);
    //basic colour startig point for the critters
    natural = ofColor(255);
    //get the listener ready to start afresh
    listen.clearTraining();
    //implementing Jukebox...
    juke.setFolderPath("./");
    //we want to make sure that the jukebox is deleting files after playing (from it's list!!)
    //if(!juke.toggleUnique())juke.toggleUnique();
    juke.toggleUnique();
}

void ofApp::seedCritters(){
    //I'm going to make a pointer to seed the random
    //makes it far more random each time
    //it's run with the use of the pointer value :)
    long tmp;
    long* tmpPnt = &tmp;
    cout<<"temp point "<<(long)tmpPnt<<"\n";
    std::pair<float,float> bassOut = listen.getOutPair('b');//returns <minBassOut,maxBassOut>
    cout<<"the pair of values are:"<<bassOut.first<<"\t:"<<bassOut.second<<"\n";
    for(int i =0;i<clouds; i++){
        int seed = ofGetElapsedTimeMicros()+i+(long)tmpPnt;
        ofSeedRandom(seed);
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


ofColor ofApp::updateColour(){
    //let's try doing the bg colour
    undercoat.b = listen.normFromBass()*255;
    undercoat.g = (listen.normFromMidHigh()*127)+(listen.normFromMidLow()*127);
    undercoat.r = listen.normFromTop()*255;
    //work out the colour based on top and mid ranges (red:mid blue:top)
    //just builds up to max colour then doesn't go back down with lerp()
    //normalize is the same as map to [0..1]
    float cShiftr = ((listen.normFromMidLow()*2)+listen.normFromBass())/3;
    //cShiftr += listen.normFromBass()/3;
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
//    std::pair<float,float> bassInPair = listen.getInPair('b');
//    std::pair<float,float> midInPair = listen.getInPair('m');
//    std::pair<float,float> midLInPair = listen.getInPair('l');
//    std::pair<float,float> topInPair = listen.getInPair('t');
    std::pair<float,float> bassOutPair = listen.getOutPair('b');
//    std::pair<float,float> midOutPair = listen.getOutPair('m');
//    std::pair<float,float> topOutPair = listen.getOutPair('t');
    //smaller than mid if toppy, bigger if bassy
    float cleanMid = listen.normFromMidLow()*bassOutPair.second;//ofMap(listen.getMidHigh(),midInPair.first,midInPair.second,bassOutPair.first,bassOutPair.second,true)/3;
    cleanMid += listen.normFromMidHigh()*bassOutPair.second;//ofMap(listen.getMidLow(),midLInPair.first,midLInPair.second,bassOutPair.first,bassOutPair.second,true)*2/3;
    float bassMid = listen.normFromBass()*bassOutPair.second;//ofMap(listen.getBass(),bassInPair.first,bassInPair.second,bassOutPair.first,bassOutPair.second,true);
    //float bassMid = ofNormalize(listen.getMidLow()+listen.getBass(),bassOutPair.first,bassOutPair.second)*cleanMid;
    float topMid = listen.normFromTop()*bassOutPair.second/2;//ofMap(listen.getTop(),topInPair.first,topInPair.second,bassOutPair.first,bassOutPair.second,true);
    //float topMid = (1-(ofNormalize(listen.getTop(),topInPair.first,topInPair.second)))*cleanMid;
    //float combiMid = max(bassMid,cleanMid);
    cleanMid -= topMid;//min(cleanMid,topMid);
    cleanMid += bassMid;///1.2;
    return cleanMid;
}

//--------------------------------------------------------------
void ofApp::update(){
    float t = ofGetElapsedTimef();
    float dt = t-time0;
    dt = ofClamp(dt,0.0,0.1);
    time0 = t;
    if(!juke.simplePlaying()){
        juke.playFile(juke.selectRandom());
        cout<<"I've finished the song!!\n";
        seedCritters();
        listen.clearTraining();
        listen.clearTempo();
    }
    //ofSoundUpdate();
    listen.updateSpectrum();
    listen.updateTempo();
    topColour = updateColour();
    topColour = natural-topColour;
    //drawing onto an ofFbo
    canvas.begin();
    fadeCanvas();
    //listen.drawSpectrum();
    float repWidth = ofGetWidth()/reps;
    for(int rep = 1; rep <= reps; rep++){
        ofPushMatrix();
        float tT = ofGetElapsedTimef();
        bool motionDir = (motionSeeds[rep-1]<0)?false:true;
        //absolute value for bass, don't want them floating up on the drop!?
        //map to get rid of lazy critters
        float mappedMotionAbs = ofMap(abs(motionSeeds[rep-1]),0,1,0.7,1);
        float mappedMotion = mappedMotionAbs;
        if(!motionDir){
            mappedMotion *= -1;
        }
        float toTwitch = (mappedMotion*twitchX)*listen.normFromTop();//(mappedMotion*fmodf(ofSignedNoise(tT),listen.getTempo()/2));
        nX = ((repWidth)*rep)-(repWidth/2);
        if(rep<= repsFloor){
            //work out x-coord and add 'personality' to their individual
            //movement based on the tempo
            nX += toTwitch;
            //fade critters as they get further away from the centre one
            topColour.a = 100+rep*(155/repsCeil);
        }else if(rep > repsCeil){
            //movement based on tempo but inverted from other side of centre
            nX -= toTwitch;
            topColour.a = 255-(fmodf(rep,repsCeil)*(155/(repsCeil)));
        }else if(rep == repsCeil){
            //no sideways movement for the centre critter?
            nX -= 1.5*toTwitch;
            topColour.a = 255;
        }
        //make the group head down with bass hit and up with top snaps
        //great, works pretty nicely first time
        yTendency = yTendency*listen.getPeakDrop();
        //so ease will be stickier if bigger
        float bassDrag =(ease*yTendency);
        bassDrag += (1-ease)*listen.normFromBass();
        float topLift = ((1-ease)*yTendency)+(ease*listen.normFromTop());
        float bDrag = (bassDrag*(listen.normFromBass()))-topLift;
        yTendency = max(yTendency,bDrag);

        float halfHt =ofGetHeight()/2;
        //new section for tempo movement -----------------------------------------------------
        float tempoYMovement;
        if(listen.isTempoSampled()){
            tempoYMovement = sin(fmodf(tT,listen.getTempo())/(listen.getTempo()/3.14))*twitchY*mappedMotion;
        }else{
            tempoYMovement = sin(fmodf(tT,3.14))*twitchY*mappedMotion;;
        }
        nY = halfHt+tempoYMovement+((0.5*twitchY)*ofSignedNoise(tT))+(((halfHt)*mappedMotionAbs)*yTendency);
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
            //destX = tx[i]*(reactionLevel*(combiMid));
            destX = ofSignedNoise(tx[i])*(reactionLevel*(combiMid));//+bass));
            //destY = ty[i]*(reactionLevel*(combiMid));//+bass));
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
    //ofBackground(70,90,70,0);
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    canvas.draw(0,0);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == OF_KEY_RETURN){
        juke.skipPlayer();
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
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
