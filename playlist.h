#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <iostream>
#include <string>
#include <vector>
#include <QString>

#include "track.h"

class Playlist
{
public:
    Playlist(QString);

    void addTrack(Track track);
    void removeTrack(QString name);
    QString getName(){return this->p_name;}
    std::vector<Track> &getTracks(){return this->p_tracks;}
    std::vector<Track>::iterator begin(){return this->p_tracks.begin();}
    std::vector<Track>::iterator end(){return this->p_tracks.end();}
    std::vector<Track>::iterator last(){return this->p_tracks.end() - 1;}
    Track& operator[](int index){return p_tracks[index];}
    bool empty(){return this->p_tracks.empty();}
private:
    std::vector<Track> p_tracks;

    QString p_name;
};

#endif // PLAYLIST_H
