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
    //get the listener ready to start afresh
    listen.clearTraining();
    //implementing Jukebox...
    juke.setFolderPath("./");
    //we want to make sure that the jukebox is deleting files after playing (from it's list!!)
    if(!juke.toggleUnique())juke.toggleUnique();
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
        //ofRandomf returns [-1..1] so this gives [-1..-0.7] or [0.7..1]
        motionSeeds[i] = ofRandom(0.7,1.0)*ofRandomf();
        cout<<"motion seed"<<i<<": "<<motionSeeds[i]<<"\n";
    }
    //I want the y movement to be affected by the music more
    yTendency = 0;
    //is it just good behaviour to clear up like this?
    tmpPnt = nullptr;
}


ofColor ofApp::updateColour(){
    //let's try doing the bg colour
    undercoat.r = (listen.normFromBass()*180)+(listen.normFromMidLow()*75);
    undercoat.g = (listen.normFromMidHigh()*180)+(listen.normFromMidLow()*75);
    undercoat.b = (listen.normFromMidHigh()*75)+(listen.normFromTop()*180);
    //work out the colour based on top and mid ranges (red:mid blue:top)
    //just builds up to max colour then doesn't go back down with lerp()
    //normalize is the same as map to [0..1]
    float cShiftr = ((listen.normFromMidLow())+listen.normFromBass())/3;//it was going too red so midLow * 1
    float cShiftb = (listen.normFromTop()/2)+(listen.normFromMidHigh()/2);
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
    std::pair<float,float> bassOutPair = listen.getOutPair('b');
    std::pair<float, float> midLOutPair = listen.getOutPair('l');
    std::pair<float, float> topOutPair = listen.getOutPair('t');
    std::pair<float, float> midOutPair = listen.getOutPair('m');
    //smaller than mid if toppy, bigger if bassy
    //essentially getting the scale of each frequency group then multiply by maxOut
    float cleanMid = listen.getMidLow();//listen.normFromMidLow()*(midLOutPair.second);
    cleanMid += listen.getMidHigh();//listen.normFromMidHigh()*(midOutPair.second);
    float bassMid = listen.getBass();//listen.normFromBass()*bassOutPair.first;
    float topMid = listen.getTop();//listen.normFromTop()*topOutPair.second;
    cleanMid -= topMid;//min(cleanMid,topMid);
    cleanMid += bassMid;
    //cleanMid += bassOutPair.first;
    return cleanMid;
}

void ofApp::resetAndStartRandomTrack()
{
    //pick another song
    juke.playFile(juke.selectRandom());
    cout << "I've finished the song!!\n";
    //run the randomiser of the critters behaviour etc
    seedCritters();
    //clear up what the listener has learnt from the last song
    listen.clearTraining();
    listen.clearTempo();
}
//--------------------------------------------------------------
void ofApp::update(){
    float t = ofGetElapsedTimef();
    float dt = t-time0;
    dt = ofClamp(dt,0.0,0.1);
    time0 = t;
    if(!juke.simplePlaying()){
        //if we've just started or a song has just finished
        resetAndStartRandomTrack();
    }
    //ofSoundUpdate();
    listen.updateSpectrum();
    listen.updateTempo();
    topColour = updateColour();
    topColour = natural-topColour;
    //drawing onto an ofFbo
    canvas.begin();
    fadeCanvas();
    float repWidth = ofGetWidth()/reps;
    for(int rep = 1; rep <= reps; rep++){
        //for each critter in the group.........
        ofPushMatrix();
        float tT = ofGetElapsedTimef();
        //absolute value for bass, don't want them floating up on the drop!?
        //map to get rid of lazy critters - need to centralise these magic floats - now in the SeedCritters()
        float mappedMotion = motionSeeds[rep-1];
        float mappedMotionAbs = abs(mappedMotion);
        //x movement caused by top end frequencies ---------------------
        float toTwitch = (mappedMotionAbs*twitchX)*(listen.normFromTop()+listen.normFromMidHigh() / 2);
        nX = ((repWidth)*rep)-(repWidth/2);
        if(rep<= repsFloor){
            //work out x-coord and add 'personality' to their individual
            //movement based on the tempo
            //making the group squeeze in from top freq. influence - lovely
            nX += (repsCeil - rep)*toTwitch;
            //fade critters as they get further away from the centre one
            topColour.a = 100+rep*(155/repsCeil);
        }else if(rep > repsCeil){
            //movement based on tempo but inverted from other side of centre
            nX -= (rep-repsCeil)*toTwitch;
            topColour.a = 255-(fmodf(rep,repsCeil)*(155/(repsCeil)));
        }else if(rep == repsCeil){
            //no sideways movement for the centre critter?
            //nX += toTwitch;
            topColour.a = 255;
        }
        //y-movement based on a plethora of frequencies and the tempo -------------------------
        //make the group head down with bass hit and up with top snaps
        //great, works pretty nicely first time
        //yTendency = yTendency*listen.getPeakDrop();
        //so ease will be stickier if bigger
        float bassDrag =(ease*yTendency);
        bassDrag += (1-ease)*listen.normFromBass();
        float topLift = ease * yTendency;
        topLift *= (1 - ease) * (listen.normFromTop()+listen.normFromMidHigh());
        float yDrag = bassDrag - topLift;//(bassDrag * (listen.normFromBass())) - topLift;
        yTendency = max(yTendency,yDrag);

        float halfHt = ofGetHeight()/2;
        //new section for tempo movement -----------------------------------------------------
        float tempoYMovement;
//        if(listen.isTempoSampled()){
            tempoYMovement = sin(fmodf(tT,listen.getTempo())/(listen.getTempo()/3.14))*twitchY*mappedMotionAbs;
//        }else{
//            tempoYMovement = sin(fmodf(tT,3.14))*twitchY*mappedMotion;;
//        }
        nY = halfHt+tempoYMovement+((0.5*twitchY)*ofSignedNoise(tT))+(((halfHt)*mappedMotion)*yTendency);
        //now we've worked out the position for the critter simply translate/rotate then draw ----------------
        ofTranslate(nX,nY);
        ofRotateDeg(360/rep);
        ofSetColor(topColour);
        //info to pass to the critter to draw itself
        float reactionLevel = mappedMotion*reactionMagnifier;
        float combiMid = calcSizeResponse();
        //these affect the speed and 'sway' of the critters
        float xr = 1+listen.normFromMidHigh()+listen.normFromTop();
        float yr = 1+listen.normFromMidLow()+listen.normFromBass();
        //break out the critter drawing, first step to making a seperate class
        drawCritter(dt,reactionLevel,combiMid,xr,yr,ease);
        ofPopMatrix();
    }
    //if you want to see the fft spectrum itself at the bottom of the screen...
    //-- this is not working when I've come back to it on Windows - 
    //listen.drawSpectrum();
    canvas.end();
}

void ofApp::drawCritter(float duration, float reaction_level, float sizing, float x_reaction, float y_reaction, float ease_value){
    ofFill();
    //make a polygon with curved edges to make it more 'organic'
    //so set the first point as an anchor
    ofPolyline critter;
    critter.addVertex(p[0]);
    for(int i = 0; i < clouds; i++){
        //how much is each point in the critter moving in this frame
        //the listen state is used to affect the movement speed and
        //kinda makes it sway with the swells in the music
        tx[i] += ((reaction_level*x_reaction)*duration);
        ty[i] += ((reaction_level*y_reaction)*duration);
        //signed noise so they don't just expand endlessly but instead throb
        //two dimensional noise adds more fluidity to the movement
        //making them local to cut ties with the main app
        float destX = ofSignedNoise(tx[i],ty[i])*sizing;
        float destY = ofSignedNoise(ty[i],tx[i])*sizing;
        //then make the actual points head towards their target position
        //if ease is small it will be twitchy, if larger, more saggy [0..1]
        p[i].x = (ease_value*p[i].x) +(1-ease_value)*(destX);
        p[i].y = (ease_value*p[i].y) +(1-ease_value)*(destY);

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
