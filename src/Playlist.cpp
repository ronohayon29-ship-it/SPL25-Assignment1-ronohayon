#include "Playlist.h"
#include "AudioTrack.h"
#include <iostream>
#include <algorithm>
Playlist::Playlist(const std::string& name) 
    : head(nullptr), playlist_name(name), track_count(0) {
    std::cout << "Created playlist: " << name << std::endl;
}
// TODO: Fix memory leaks!
// Students must fix this in Phase 1
Playlist::~Playlist() {
    PlaylistNode* current = head;
    while(current!=nullptr){
        PlaylistNode* next = current->next;
        delete current->track;
        delete current;
        current = next;
    }
    head = nullptr;
    #ifdef DEBUG
    std::cout << "Destroying playlist: " << playlist_name << std::endl;
    #endif
}

void Playlist::add_track(AudioTrack* track) {
    if (!track) {
        std::cout << "[Error] Cannot add null track to playlist" << std::endl;
        return;
    }

    // Create new node - this allocates memory!
    PlaylistNode* new_node = new PlaylistNode(track);

    // Add to front of list
    new_node->next = head;
    head = new_node;
    track_count++;

    std::cout << "Added '" << track->get_title() << "' to playlist '" 
              << playlist_name << "'" << std::endl;
}

void Playlist::remove_track(const std::string& title) {
    PlaylistNode* current = head;
    PlaylistNode* prev = nullptr;

    // Find the track to remove
    while (current && current->track->get_title() != title) {
        prev = current;
        current = current->next;
    }

    if (current) {
        // Remove from linked list
        if (prev) {
            prev->next = current->next;
        } else {
            head = current->next;
        }

        track_count--;
        delete current->track;
        delete current;
        std::cout << "Removed '" << title << "' from playlist" << std::endl;

    } else {
        std::cout << "Track '" << title << "' not found in playlist" << std::endl;
    }
}

void Playlist::display() const {
    std::cout << "\n=== Playlist: " << playlist_name << " ===" << std::endl;
    std::cout << "Track count: " << track_count << std::endl;

    PlaylistNode* current = head;
    int index = 1;

    while (current) {
        std::vector<std::string> artists = current->track->get_artists();
        std::string artist_list;

        std::for_each(artists.begin(), artists.end(), [&](const std::string& artist) {
            if (!artist_list.empty()) {
                artist_list += ", ";
            }
            artist_list += artist;
        });

        AudioTrack* track = current->track;
        std::cout << index << ". " << track->get_title() 
                  << " by " << artist_list
                  << " (" << track->get_duration() << "s, " 
                  << track->get_bpm() << " BPM)" << std::endl;
        current = current->next;
        index++;
    }

    if (track_count == 0) {
        std::cout << "(Empty playlist)" << std::endl;
    }
    std::cout << "========================\n" << std::endl;
}

AudioTrack* Playlist::find_track(const std::string& title) const {
    PlaylistNode* current = head;

    while (current) {
        if (current->track->get_title() == title) {
            return current->track;
        }
        current = current->next;
    }

    return nullptr;
}

int Playlist::get_total_duration() const {
    int total = 0;
    PlaylistNode* current = head;

    while (current) {
        total += current->track->get_duration();
        current = current->next;
    }

    return total;
}

std::vector<AudioTrack*> Playlist::getTracks() const {
    std::vector<AudioTrack*> tracks;
    PlaylistNode* current = head;
    while (current) {
        if (current->track)
            tracks.push_back(current->track);
        current = current->next;
    }
    return tracks;
}

//move assigment operator
Playlist& Playlist::operator=(Playlist&& other) noexcept {
    if (this == &other)
        return *this;


    PlaylistNode *curr = head;
    while(curr){
        PlaylistNode *next = curr->next;
        delete curr->track;
        delete curr;
        curr = next;
    }
    playlist_name = std::move(other.playlist_name);
    head = other.head;
    track_count = other.track_count;

    other.head = nullptr;
    other.track_count = 0;
    other.playlist_name.clear();

    return *this;
}

Playlist::Playlist(Playlist&& other) noexcept
    : 
      head(other.head),
      playlist_name(std::move(other.playlist_name)),
      track_count(other.track_count)
{
    other.head = nullptr;
    other.track_count = 0;
}


// copy
Playlist::Playlist(const Playlist &other) : head(nullptr), playlist_name(other.playlist_name), track_count(0)
{
#ifdef DEBUG
    std::cout << "Copy Constructor for playlist: " << playlist_name << std::endl;
#endif

    PlaylistNode *current = other.head;
    std::vector<PlaylistNode *> nodes;
    while (current)
    {
        nodes.push_back(current);
        current = (*current).next;
    }
    for (int i = nodes.size() - 1; i >= 0; i--)
    {
        AudioTrack *clone = nodes[i]->track->clone().release();
        add_track(clone);
    }
}


Playlist &Playlist::operator=(const Playlist &other)
{
#ifdef DEBUG
    std::cout << "Copy Assginment Operator for playlist: " << playlist_name << std::endl;
#endif

    if (this == &other)
    {
        return *this;
    }

    playlist_name = other.playlist_name;
    track_count = 0;

    PlaylistNode *this_current = head;
    PlaylistNode *this_prev = nullptr;

    while (this_current)
    {
        this_prev = this_current;
        this_current = this_current->next;

        if (this_prev)
        {
            if ((*this_prev).track)
            {
                delete (*this_prev).track;
            }
            delete this_prev;
        }
    }

    head = nullptr;

    std::vector<PlaylistNode *> nodes;
    PlaylistNode *current = other.head;

    while (current)
    {
        nodes.push_back(current);
        current = current->next;
    }

    for (int i = nodes.size() - 1; i >= 0; --i)
    {
        AudioTrack *clone = nodes[i]->track->clone().release();
        add_track(clone);
    }

    return *this;
}

