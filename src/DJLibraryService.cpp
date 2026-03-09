#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>


DJLibraryService::DJLibraryService(const Playlist& playlist) 
    : playlist(playlist), library() {}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo>& library_tracks) {
    //Todo: Implement buildLibrary method
    AudioTrack* curr = nullptr;
    for(const SessionConfig::TrackInfo& info: library_tracks){
        if(info.type == "MP3"){
            curr = new MP3Track(
                info.title,
                info.artists,
                info.duration_seconds,
                info.bpm,
                info.extra_param1,
                info.extra_param2 != 0);
        }
        else{
            curr = new WAVTrack(
                info.title,
                info.artists,
                info.duration_seconds,
                info.bpm,
                info.extra_param1,
                info.extra_param2);
        }
        library.push_back(curr);
    }
    std::cout << "[INFO] Track library built: "<< library.size()<< " tracks loaded"<< std::endl;
    curr = nullptr;
}

/**
 * @brief Display the current state of the DJ library playlist
 * 
 */
void DJLibraryService::displayLibrary() const {
    std::cout << "=== DJ Library Playlist: " 
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty()) {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 * 
 * @return Playlist& 
 */
Playlist& DJLibraryService::getPlaylist() {
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 * 
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack* DJLibraryService::findTrack(const std::string& track_title) {
    // Your implementation here
    return playlist.find_track(track_title); // Placeholder
}

void DJLibraryService::loadPlaylistFromIndices(const std::string& playlist_name, 
                                               const std::vector<int>& track_indices) {
    // Your implementation here
    std::cout << "[INFO] Loading playlist: " << playlist_name << std::endl;
    Playlist p(playlist_name);
    for(size_t i = 0; i<track_indices.size(); ++i){
        int index = track_indices[i];
        if(index-1<0 || static_cast<size_t>(index) > library.size()){
            std::cout << "[WARNING] Invalid track index: " << index << std::endl;
            continue;
        }
        else{
            PointerWrapper<AudioTrack> clone = library[index-1]->clone();
            if(!clone){
                std::cout << "[ERROR] Failed to clone track at index: " << index << std::endl;
                continue;
            }
            AudioTrack* cloned_track = clone.release();
            cloned_track->load();
            cloned_track->analyze_beatgrid();
            p.add_track(cloned_track);
          /*  std::cout << "[INFO] Added '" << cloned_track->get_title()
                  << "' to playlist '" << playlist_name << "'" << std::endl; */
            }
    }
    playlist = std::move(p);
    std::cout << "[INFO] Playlist loaded: " << playlist_name
              << " (" << playlist.get_track_count() << " tracks)" << std::endl;

    // For now, add a placeholder to fix the linker error
}
/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const {
    // Your implementation here
    std::vector<AudioTrack*> tracks = playlist.getTracks();
    std::vector<std::string> names;
    for(size_t i=0;i<tracks.size();++i){
        names.push_back(tracks[i]->get_title());
    }
    return names;
}

//destructor
DJLibraryService::~DJLibraryService() {
    for (AudioTrack* track : library) {
        delete track;
    }
    library.clear();
}
