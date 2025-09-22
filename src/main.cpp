#include <SFML/Audio.hpp>
#include <iostream>
#include <ncurses.h>
#include <thread>
#include <chrono>
#include "song.h"
#include "playlist.h"
using namespace std;
using namespace sf;

SongLibrary library;
PlaylistLibrary playlists;

void playSong(SongBST* s) {
    if(!s) return;
    Music music;
    if(!music.openFromFile(s->data.filepath)) { cout << "Cannot open: " << s->data.filepath << endl; return; }
    music.play();
    cout << "Playing: " << s->data.name << " by " << s->data.artist << "\n";
    cout << "Controls: P/p=Pause, Q/q=Quit\n";
    char c;
    while(music.getStatus()==SoundSource::Status::Playing || music.getStatus()==SoundSource::Status::Paused) {
        this_thread::sleep_for(chrono::milliseconds(200));
        if(cin.peek()!=EOF) { cin >> c;
            if(c=='q'||c=='Q') { music.stop(); break; }
            if(c=='p'||c=='P') { if(music.getStatus()==SoundSource::Status::Playing) music.pause(); else music.play();}
        }
    }
}

void playPlaylist(PlaylistNode* node) {
    if(!node) return;
    Music music;
    char c;
    while(node){
        if(!music.openFromFile(node->song->data.filepath)) { cout << "Cannot open: " << node->song->data.filepath << endl; return; }
        music.play();
        cout << "Playing: " << node->song->data.name << " by " << node->song->data.artist << "\n";
        cout << "Controls: P/p=Pause, N/n=Next, B/b=Prev, Q/q=Quit\n";
        bool nextSong=false;
        while(music.getStatus()==SoundSource::Status::Playing || music.getStatus()==SoundSource::Status::Paused){
            this_thread::sleep_for(chrono::milliseconds(200));
            if(cin.peek()!=EOF){ cin >> c;
                if(c=='q'||c=='Q') { music.stop(); return; }
                else if(c=='p'||c=='P'){ if(music.getStatus()==SoundSource::Status::Playing) music.pause(); else music.play(); }
                else if((c=='n'||c=='N') && node->next){ music.stop(); node=node->next; nextSong=true; break; }
                else if((c=='b'||c=='B') && node->prev){ music.stop(); node=node->prev; nextSong=true; break; }
            }
        }
        if(!nextSong) node=node->next;
    }
}

int main() {
    library.LoadLibrary();
    playlists.LoadAllPlaylists(library);

    while(true){
        cout << "\n1.Add Song\n2.Play Song\n3.Create Playlist\n4.Add Song to Playlist\n5.Show Playlist\n6.Play Playlist\n7.Save & Exit\nChoice: ";
        int ch; cin >> ch;
        
        cin.ignore();
        if(ch==1){
            string n,a,al,p; int d;
            cout << "Name: "; getline(cin,n);
            cout << "Artist: "; getline(cin,a);
            cout << "Album: "; getline(cin,al);
            cout << "Duration(sec): "; cin >> d; cin.ignore();
            cout << "Filepath (leave empty for Music folder): "; getline(cin,p);
            SongData s(n,a,al,d,p); library.AddSong(s);
        }
        else if(ch==2){
            string name; cout << "Song name: "; getline(cin,name);
            SongBST* s = library.FindSong(name);
            if(!s) cout << "Not found\n";
            else playSong(s);
        }
        else if(ch==3){
            string name; cout << "Playlist name: "; getline(cin,name);
            playlists.AddPlaylist(name);
        }
        else if(ch==4){
            string plname,songname;
            cout << "Playlist name: "; getline(cin,plname);
            cout << "Song name: "; getline(cin,songname);
            Playlist* pl = playlists.FindPlaylist(plname);
            SongBST* s = library.FindSong(songname);
            if(pl && s) playlists.AddSongToPlaylist(pl,s);
        }
        else if(ch==5){
            string plname; cout << "Playlist name: "; getline(cin,plname);
            Playlist* pl = playlists.FindPlaylist(plname);
            playlists.ShowPlaylist(pl);
        }
        else if(ch==6){
            string plname; cout << "Playlist name: "; getline(cin,plname);
            Playlist* pl = playlists.FindPlaylist(plname);
            if(pl && pl->head) playPlaylist(pl->head);
        }
        else if(ch==7){
            library.SaveLibrary();
            playlists.SaveAllPlaylists();
            cout << "Saved. Goodbye!\n"; break;
        } else 
        {
            cout << "please enter a valid number!";
          break;
        }
        
        
    }
}
