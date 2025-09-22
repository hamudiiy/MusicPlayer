#ifndef SONG_H
#define SONG_H
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
using namespace std;
namespace fs = std::filesystem;

struct SongData {
    string name, artist, album, filepath;
    int duration; // seconds

    SongData(string n, string a, string al, int d, string path = "")
        : name(n), artist(a), album(al), duration(d) {
        fs::path musicFolder = fs::current_path() / "Music";
        if (!fs::exists(musicFolder)) fs::create_directory(musicFolder);
        filepath = path.empty() ? (musicFolder / (name + ".ogg")).string() : path;
    }
};

struct SongBST {
    SongData data;
    SongBST *left, *right;
    SongBST(SongData s) : data(s), left(nullptr), right(nullptr) {}
};

class SongLibrary {
    SongBST* root;

    SongBST* Insert(SongBST* node, SongData song) {
        if (!node) return new SongBST(song);
        if (song.name < node->data.name) node->left = Insert(node->left, song);
        else if (song.name > node->data.name) node->right = Insert(node->right, song);
        return node;
    }

    SongBST* Search(SongBST* node, const string& name) {
        if (!node || node->data.name == name) return node;
        if (name < node->data.name) return Search(node->left, name);
        return Search(node->right, name);
    }

    void Sort(SongBST* node) 
{
        if (!node) return;
        Sort(node->left);
        mvprintw(getcury(stdscr)+1, 1, "%s by %s", node->data.name.c_str(), node->data.artist.c_str());
        Sort(node->right);
}


    void SaveNode(ofstream& out, SongBST* node) {
        if (!node) return;
        SaveNode(out, node->left);
        out << node->data.name << '|'
            << node->data.artist << '|'
            << node->data.album << '|'
            << node->data.duration << '|'
            << node->data.filepath << '\n';
        SaveNode(out, node->right);
    }

public:
    SongLibrary() : root(nullptr) {}
    void AddSong(SongData s) { root = Insert(root, s); }
    SongBST* FindSong(const string& name) { return Search(root, name); }
    void Display() {Sort(root);}

    void SaveLibrary() {
        fs::create_directories("Data");
        ofstream out("Data/songs.txt");
        if (!out) return;
        SaveNode(out, root);
    }

    void LoadLibrary() {
        ifstream in("Data/songs.txt");
        if (!in) return;
        string line;
        while (getline(in, line)) {
            if (line.empty()) continue;
            string name, artist, album, durStr, path;
            size_t pos = 0;
            pos = line.find('|'); name = line.substr(0,pos); line.erase(0,pos+1);
            pos = line.find('|'); artist = line.substr(0,pos); line.erase(0,pos+1);
            pos = line.find('|'); album = line.substr(0,pos); line.erase(0,pos+1);
            pos = line.find('|'); durStr = line.substr(0,pos); line.erase(0,pos+1);
            path = line;
            int dur = stoi(durStr);
            AddSong(SongData(name, artist, album, dur, path));
        }
    }
};

#endif
