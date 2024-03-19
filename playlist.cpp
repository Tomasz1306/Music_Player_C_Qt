#include "playlist.h"


Playlist::Playlist(QString name) : p_name(name)
{

}
void Playlist::addTrack(Track track){
    this->p_tracks.push_back(track);
}
void Playlist::removeTrack(QString name){
    for(auto it = this->p_tracks.begin(); it != this->p_tracks.end(); it++){
        if(it->getName() == name){
            this->p_tracks.erase(it);
            break;
        }
    }
}
