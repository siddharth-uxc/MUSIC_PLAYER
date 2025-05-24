#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main() {
    vector<string> playlist;
    ifstream file("playlist.txt");
    string songName;

    // Read all songs from playlist.txt
    while (getline(file, songName)) {
        if (!songName.empty()) {
            playlist.push_back(songName);
        }
    }

    if (playlist.empty()) {
        cout << "Playlist is empty or playlist.txt not found.\n";
        return -1;
    }

    sf::Music music;
    for (const auto& song : playlist) {
        cout << "Now playing: " << song << "\n";

        if (!music.openFromFile(song)) {
            cout << "Could not open file: " << song << "\n";
            continue;
        }

        music.play();
        cout << "Press Enter to play the next song...\n";
        cin.get();  // Wait for user to press Enter
        music.stop();
    }

    cout << "All songs finished.\n";
    return 0;
}
