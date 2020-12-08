#ifndef JUKEBOX_H
#define JUKEBOX_H
#include "ofMain.h"

class Jukebox : public ofBaseApp
{
    //maybe a collection of 12 players, like a pool?
    vector<ofSoundPlayer*> playerGroup;
    ofSoundPlayer player;
    vector<ofFile> tList;
    ofDirectory tunes;
    int jbSize{0};
    /*
     * unique_mode: to make it a random player that only
     * plays each file once until the collection is empty
     * at which point it refills and continues
     * multi_mode: to set up a pool of players that are
     * selected to play if they are available when required
     * index_players: I want to set it up so that each
     * player in the group has one file loaded and utilise
     * built-in multiplay
     */
    bool unique_mode{false}, multi_mode{false}, index_players{false};

public:
    Jukebox();
    Jukebox(string folder_path);
    ~Jukebox();
    void makeMulti(int nOfPlayers);
    ofFile selectRandom();
    ofFile selectByIndex(int);
    //volume has default value of 1.0
    //true if player has a spare player
    //if only one then poll this to check
    //whether it has finished playing
    //pan [-1..1] = [L..R]
    bool playFile(ofFile, float volumeNormalised = 1.0, float pan =0.0);
    //associates each player with a file
    bool setUpSamples();
    bool playByIndex(int index, float volumeNormalised = 1.0, float pan = 0.0);
    bool setFolderPath(string folder_path);
    //true to remove track after playing until
    //tList is empty, then refill
    bool toggleUnique();
    bool isMultiMode(){return multi_mode;}
    bool isSamplerMode(){return index_players;}
    void update() override;
};

#endif // JUKEBOX_H
