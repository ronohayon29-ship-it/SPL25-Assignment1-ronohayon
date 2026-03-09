#include "DJControllerService.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>

DJControllerService::DJControllerService(size_t cache_size)
    : cache(cache_size) {}
/**
 * TODO: Implement loadTrackToCache method
 */
int DJControllerService::loadTrackToCache(AudioTrack& track) {
    // Your implementation here 
    if(cache.contains(track.get_title())){
        cache.get(track.get_title());
        displayCacheStatus();
        return 1;
    }
    PointerWrapper<AudioTrack> clone = track.clone();
    if(!clone){
        std::cerr << "[ERROR] Track: \"" << track.get_title()
              << "\" failed to clone" << std::endl;
        return 0;
    }
    AudioTrack* clone_raw = clone.get();
    clone_raw->load();
    clone_raw->analyze_beatgrid();
    PointerWrapper<AudioTrack> new_clone = std::move(clone);
    bool evicted = cache.put(std::move(new_clone));
    displayCacheStatus();
    if(evicted){
        return -1;
    }
    return 0; // Placeholder
}

void DJControllerService::set_cache_size(size_t new_size) {
    cache.set_capacity(new_size);
}
//implemented
void DJControllerService::displayCacheStatus() const {
    std::cout << "\n=== Cache Status ===\n";
    cache.displayStatus();
    std::cout << "====================\n";
}

/**
 * TODO: Implement getTrackFromCache method
 */
AudioTrack* DJControllerService::getTrackFromCache(const std::string& track_title) {
    // Your implementation here
    return cache.get(track_title); // Placeholder
}
