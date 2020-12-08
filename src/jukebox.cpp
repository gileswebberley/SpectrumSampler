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

bool Jukebox::toggleUnique(){
    unique_mode = !unique_mode;
    return unique_mode;
}

void Jukebox::makeMulti(int nOfPlayers){
    //check arg is reasonable...
    if(nOfPlayers<0||nOfPlayers>32){
        cerr<<"Jukebox: why did you ask for "<<nOfPlayers<<" players?? Must be in range [0..32]\n";
        return;
    }
    int pgs = playerGroup.size();
    //check the player pool hasn't already been made big enough
    if(multi_mode && nOfPlayers<=pgs){
        cerr<<"Jukebox: enough players already\n";
        return;
    }
    //if we want less than we've got clear up
    if(multi_mode && nOfPlayers<pgs){
        pgs -= nOfPlayers;
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
    index = (index>=jbSize)?index%jbSize:index;
    ofSoundPlayer* sample = playerGroup.at(index);
    sample->setPan(pan);
    sample->setVolume(vn);
    sample->play();
    cout<<"JUKEBOX: playing sample #"<<index<<" position\n";
    return true;
}

//use for isPlaying() style functionality
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
        cerr<<"multi-mode not enabled\n";
        return false;
    }

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

ofFile Jukebox::selectRandom(){
    int me = 2;
    int* seedPtr = &me;
    //make it as random as possible
    int seed = ofGetElapsedTimeMicros()+(long)seedPtr;
    ofSeedRandom(seed);
    int selectTune = floor(ofRandom(1.0)*(jbSize-1));
    if(unique_mode){
        //whether to play each once
        tList.erase(tList.begin()+selectTune);
        tList.shrink_to_fit();
        cout<<"Jukebox has: "<<tList.size()<<" tracks loaded\n";
        if(tList.empty()){
            //if they've all been played in unique mode then refill collection
            tList = tunes.getFiles();
        }
    }

    return tList.at(selectTune);
}

void Jukebox::update(){
    ofSoundUpdate();
}
