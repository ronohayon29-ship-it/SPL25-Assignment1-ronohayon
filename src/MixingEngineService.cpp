#include "MixingEngineService.h"
#include <iostream>
#include <memory>


/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
    : decks(), active_deck(0), auto_sync(false), bpm_tolerance(0)
{
    // Your implementation here
    decks[0] = nullptr;
    decks[1] = nullptr;
    std::cout << "[MixingEngineService] Initialized with 2 empty decks." << std::endl;
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService() {
    // Your implementation here
    std::cout << "[MixingEngineService] Cleaning up decks...";
    for (int i = 0; i < 2; i++) {
        if (decks[i] != nullptr) {
            delete decks[i];
            decks[i] = nullptr;
        }
    }
}


/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack& track) {
    // Your implementation here
    std::cout << "\n=== Loading Track to Deck ===" << std::endl;
    PointerWrapper<AudioTrack> toInsert = track.clone();
    if(!toInsert){
        std::cout << "[ERROR] Track: \"" << track.get_title() << "\" failed to clone" << std::endl;
        return -1;
    }
    size_t target = 3; // value that doesnt exist in decks
    if(decks[0]==nullptr && decks[1]==nullptr){
        target = 0;
    }
    else{
        target = 1-active_deck;
    }
    std::cout << "[Deck Switch] Target deck: " << target << std::endl;
    if(decks[target]!=nullptr){
        delete decks[target];
        decks[target] = nullptr;
    }
    toInsert.get()->load();
    toInsert.get()->analyze_beatgrid();
    if(decks[1-target]!=nullptr&&auto_sync&&!can_mix_tracks(toInsert)){
        sync_bpm(toInsert);
    }

    if(decks[active_deck]!=nullptr&&decks[target]!=nullptr){
        std::cout << "[Unload] Unloading previous deck " 
        << active_deck 
        << " (" << decks[active_deck]->get_title() << ")"
        << std::endl;
        delete decks[target];
        decks[target] = nullptr;
    }
    if(decks[0]==nullptr && decks[1]==nullptr){
        std::cout << "[Sync BPM] Cannot sync - one of the decks is empty." << std::endl;
    }

    decks[target] = toInsert.release();
    std::cout << "[Load Complete] '"
          << track.get_title()
          << "' is now loaded on deck "
          << target
          << std::endl;
    
    active_deck = target;
    std::cout << "[Active Deck] Switched to deck "
          << target
          << std::endl;
    displayDeckStatus();
    return target; // Placeholder
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const {
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i) {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 * 
 * Check if two tracks can be mixed based on BPM difference.
 * 
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack>& track) const {
    // Your implementation here
    if(decks[active_deck]==nullptr || !track){
        return false;
    }
    int bpm_active = decks[active_deck]->get_bpm();
    int bpm_track = track.get()->get_bpm();
    return std::abs(bpm_active-bpm_track)<=bpm_tolerance;

}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack>& track) const {
    // Your implementation here
    if(decks[active_deck]==nullptr || !track){
        return;
    }
    int bpm_active = decks[active_deck]->get_bpm();
    int bpm_track = track.get()->get_bpm();
    int avg = (bpm_active+bpm_track)/2;
    track->set_bpm(avg);
    std::cout << "[Sync BPM] Syncing BPM from "<< bpm_track << " to " << avg << std::endl;
}
