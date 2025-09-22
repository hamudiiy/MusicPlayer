#ifndef PLAYLIST_H
#define PLAYLIST_H
#include "song.h"
#include <filesystem>
#include <fstream>
#include <string>
using namespace std;
namespace fs = std::filesystem;

struct PlaylistNode {
    SongBST* song;
    PlaylistNode* next;
    PlaylistNode* prev;
    PlaylistNode(SongBST* s) : song(s), next(nullptr), prev(nullptr) {}
};

struct Playlist {
    string name;
    PlaylistNode* head;
    PlaylistNode* tail;
    Playlist* next;
    Playlist* prev;
    Playlist(string n) : name(n), head(nullptr), tail(nullptr), next(nullptr), prev(nullptr) {}
};

class PlaylistLibrary {
    Playlist* head;
    Playlist* tail;
public:
    PlaylistLibrary() : head(nullptr), tail(nullptr) {}

    void AddPlaylist(const string& name) {
        Playlist* pl = new Playlist(name);
        if (!head) head = tail = pl;
        else { tail->next = pl; pl->prev = tail; tail = pl; }
    }

    Playlist* FindPlaylist(const string& name) {
        Playlist* temp = head;
        while(temp) {
            if(temp->name == name) return temp;
            temp = temp->next;
        }
        return nullptr;
    }

    void AddSongToPlaylist(Playlist* pl, SongBST* song) {
        if (!pl || !song) return;
        PlaylistNode* node = new PlaylistNode(song);
        if (!pl->head) pl->head = pl->tail = node;
        else { pl->tail->next = node; node->prev = pl->tail; pl->tail = node; }
      
    }

    void ShowPlaylist(Playlist* pl) {
        if (!pl || !pl->head) { cout << "Empty playlist\n"; return; }
        cout << "Playlist: " << pl->name << "\n";
        for(PlaylistNode* n = pl->head; n; n = n->next)
            cout << "- " << n->song->data.name << " by " << n->song->data.artist << "\n";
    }

    void SaveAllPlaylists() {
        fs::create_directories("Data");
        Playlist* cur = head;
        while(cur) {
            ofstream out("Data/" + cur->name + ".txt");
            for(PlaylistNode* n = cur->head; n; n = n->next)
                out << n->song->data.name << "\n";
            cur = cur->next;
        }
    }

    void LoadAllPlaylists(SongLibrary& lib) {
        fs::create_directories("Data");
        for (auto& p : fs::directory_iterator("Data")) {
            if (!p.is_regular_file()) continue;
            string fname = p.path().filename().string();
            if(fname == "songs.txt") continue;
            string plname = p.path().stem().string();
            AddPlaylist(plname);
            Playlist* pl = FindPlaylist(plname);
            ifstream in(p.path());
            string sname;
            while(getline(in, sname)) {
                SongBST* s = lib.FindSong(sname);
                if(s) {
           
                    PlaylistNode* node = new PlaylistNode(s);
                    if(!pl->head) pl->head = pl->tail = node;
                    else { pl->tail->next = node; node->prev = pl->tail; pl->tail = node; }
                }
            }
        }
    }

    Playlist* GetHead() { return head; }
};

#endif
