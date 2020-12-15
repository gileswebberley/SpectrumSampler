#include "jukebox.h"

Jukebox::Jukebox()
{
    Jukebox("./");
}

Jukebox::Jukebox(string fp){
    if(setFolderPath(fp))
    {
        cout<<"Jukebox created\n\n";
    }else//failed to load any files
    {
        cerr<<"Jukebox failed to load any files\n\n";
    }
    //do I need multiple players somehow??
    player.setMultiPlay(true);
    player.setLoop(false);
}

Jukebox::~Jukebox(){
    if(multi_mode){
        //I thought I could use delete[] ??
        for(ofSoundPlayer* sp:playerGroup){
            delete sp;
        }
    }
}

bool Jukebox::setFolderPath(string folder_path){
    tunes.open(folder_path);
    tunes.allowExt("wav");
    tunes.allowExt("mp3");
    tunes.listDir();
    tList = tunes.getFiles();
    jbSize = tList.size();
    //check that it found some files
    if(tList.empty()){
        cerr<<"no music files found in "<<folder_path<<"\n";
        return false;
    }
    //+ reset sample player system
    if(index_players) setUpSamples();
    return true;
}

ofFile Jukebox::selectByIndex(int i){
    //check arg is in bounds
    if(i>int(tList.size()-1))
    {//if > roll around to start
        i = i%tList.size();
    }else if(i<0)
    {//assume a negative is 0
        i = 0;
    }
    return tList.at(i);
}

//to set up playByIndex behaviour
bool Jukebox::setUpSamples()
{
    //make a player for each file in the samples folder
    //remember makeMulti looks after no_loop and multi_play
    makeMulti(jbSize);
    multi_mode = false;
    index_players = true;
    for(int i=0; i<jbSize; i++){
        //pre-load the samples in each
        playerGroup.at(i)->load(selectByIndex(i));
    }
    cout<<"JUKEBOX: set up in sampler mode\n";
    return true;
}

//use setUpSamples(), which uses selectByIndex() for bounds safe loading, to use this functionality
bool Jukebox::playByIndex(int index, float vn, float pan){
    if(!index_players){
        //if we're not set up by index samples then convert to
        //a play by file call
        cerr<<"JUKEBOX: Play by index system not initialised before calling playByIndex...use setUpSampler()\n";
        //use bounds checking of selectByIndex()
        return playFile(selectByIndex(index),vn,pan);
    }
    //get the index in bounds
    index = (index<0)?abs(index):index;
    //using jbSize as it's an int rather than size_t
    //careful, jbSize is effected by unique_mode behaviour
    //finally using playerGroup.size as that is what we're
    //trying not to go out of bounds with after all?
    index = (index>=(int)playerGroup.size())?index%(int)playerGroup.size():index;
    ofSoundPlayer* sample = playerGroup.at(index);
    sample->setPan(pan);
    sample->setVolume(vn);
    sample->play();
    cout<<"JUKEBOX: playing sample #"<<index<<" position\n";
    return true;
}

ofFile Jukebox::selectRandom(){
    int me = 2;
    int* seedPtr = &me;
    //make it as random as possible
    int seed = ofGetElapsedTimeMicros()+(long)seedPtr;
    ofSeedRandom(seed);
    int selectTune = floor(ofRandom(1.0)*(tList.size()-1));

    ofFile tmpTune = tList.at(selectTune);
    if(unique_mode && !index_players){
        //whether to play each once
        cout<<"Jukebox has: "<<tList.size()<<" tracks, deleting track: "<<tmpTune.getBaseName()<<"\n";
        tList.erase(tList.begin()+selectTune);
        tList.shrink_to_fit();
        if(tList.empty()){
            //if they've all been played in unique mode then refill collection
            tList = tunes.getFiles();
        }
        //ensure jbSize reflects current size of the directory list
        jbSize = tList.size();
    }
    //there, found a crappy bug, hadn't even noticed up til now
    //we were returning tList.at(selectTune) which had been deleted!!?
    cout<<"Track now at the selected position: "<<tList.at(selectTune).getBaseName()<<"\n";
    return tmpTune;
}

bool Jukebox::toggleUnique(){
    unique_mode = !unique_mode;
    return unique_mode;
}

void Jukebox::makeMulti(int nOfPlayers){
    //check arg is reasonable...
    if(nOfPlayers<0||nOfPlayers>32){
        cerr<<"Jukebox: multi-players must be in range [0..32]\n";
        return;
    }
    int pgs = playerGroup.size();
    //check the player pool hasn't already been made big enough
    if(multi_mode && nOfPlayers==pgs){
        cerr<<"Jukebox: has "<<nOfPlayers<<" players already\n";
        return;
    }
    //if we want less than we've got clear up
    if(multi_mode && nOfPlayers<pgs){
        pgs -= nOfPlayers;
        cerr<<"Jukebox: shrank multi-player capacity by "<<pgs<<" players\n";
        while(pgs > 0){
            //delete the pointer to player
            delete playerGroup.back();
            //remove the container element
            playerGroup.pop_back();
            --pgs;
        }
        playerGroup.shrink_to_fit();
        return;
    }
    //add extra players if current group is smaller
    nOfPlayers = nOfPlayers-playerGroup.size();
    multi_mode = true;
    index_players = false;
    for(int cnt=0;cnt<nOfPlayers;cnt++){
        playerGroup.push_back(new ofSoundPlayer);
        playerGroup[cnt]->setMultiPlay(true);
        playerGroup[cnt]->setLoop(false);
        cout<<"new sound player:["<<cnt<<"]\t";
    }
    playerGroup.shrink_to_fit();
    cout<<"\nMultiplay capacity: "<<playerGroup.size()<<"\n";
}

/*
 * Now that i'm trying to use it as the jukebox it was
 * originally intende to be I need to do some workarounds
 */
void Jukebox::skipPlayer(){
    if(!multi_mode){
        player.stop();
        player.setPosition(1.0);
    }
}

//use for isPlaying() style functionality
bool Jukebox::simplePlaying(){
    if(!multi_mode)return player.isPlaying();
    else{
        if(!player.isPlaying()){
            return false;
        }else{
            for (ofSoundPlayer* sp : playerGroup){
                if(!sp->isPlaying())return false;
            }
        }
        //every player is busy so playergroup.isPlaying() equiv.
        return true;
    }
}

//returns success of play request
bool Jukebox::playFile(ofFile f, float vol, float p){
    if(!player.isPlaying()){
        cout<<"player is available\n";
        player.load(f);
        player.setVolume(vol);
        player.setPan(p);
        player.play();
        return true;
    }else if(multi_mode && !index_players){
        //why isn't  && !index_players working???
        //don't want to mess with the group if it's been set up as
        //an array of preloaded sample files hence !index_players
        for(ofSoundPlayer* sp : playerGroup){
            //check for a free player
            //cout<<"check player..\t";
            if(!sp->isPlaying()){
                //sp->unload();
                cout<<"multi player is available\t";
                sp->load(f);
                sp->setVolume(vol);
                sp->setPan(p);
                sp->play();
                return true;
            }else{
                //cerr<<"multi player is busy\n";
            }
        }
        cerr<<"all players are busy\n";
        return false;
    }else{
        //it's not really an error cos it's just using
        //it as originally intended
        //cerr<<"multi-mode not enabled\n";
        return false;
    }

}


void Jukebox::update(){
    ofSoundUpdate();
}
