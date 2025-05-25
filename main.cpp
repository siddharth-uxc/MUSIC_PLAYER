    #include <SFML/Audio.hpp>   //for music audio function 
    #include <iostream>              //input/output 
    #include <vector>      //playlist 
    #include <fstream>       // I/O for laoding playlist.txt
    #include <string>
    #include <algorithm>
    #include <cstdio>
    #include <thread>     //for timer
    #include <atomic>
    #include <chrono>     //seconds for  timer sleep 

    using namespace std;

    atomic<bool> showTimer{false};           // Shared flag to control timer thread

    vector<string> loadPlaylist(const string& filename) {
        vector<string> playlist;
        ifstream file(filename);
        string line;

        if (!file) {
            cout << "Could not open " << filename << endl;
            return playlist;
        }

        while (getline(file, line)) {
            if (!line.empty()) playlist.push_back(line);
        }

        file.close();
        return playlist;
    }

    string formatTime(sf::Time time) {
        int totalSeconds = static_cast<int>(time.asSeconds());
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        char buffer[6];
        snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
        return string(buffer);
    }

    void timerThread(sf::Music* music) {
        while (showTimer && music->getStatus() != sf::Music::Stopped) {
            this_thread::sleep_for(chrono::seconds(1));
            sf::Time current = music->getPlayingOffset();
            sf::Time total = music->getDuration();
            cout << "\r[" << formatTime(current) << " / " << formatTime(total) << "] ";
            cout.flush();
        }
    }

    int playbackMenu(sf::Music& music, const string& filename, int currentIndex, int playlistSize) {
        cout << "\nNow playing: " << filename << "\n";
        music.play();
        showTimer = true;
        thread t(timerThread, &music);  // Start timer in background

        while (music.getStatus() != sf::Music::Stopped && showTimer) {
            cout << "\n\n[p]ause | [r]esume | [+]/[-] volume | [n]ext | [b]ack | [s]top";
            cout << "\nEnter command: ";

            string input;
            getline(cin, input);

            if (input.empty()) continue;

            char cmd = input[0];

            if (cmd == 'p') {
                music.pause();
                cout << "Paused.\n";
            } else if (cmd == 'r') {
                music.play();
                cout << "Resumed.\n";
            } else if (cmd == '+') {
                float vol = min(100.f, music.getVolume() + 10);
                music.setVolume(vol);
                cout << "Volume: " << vol << "%\n";
            } else if (cmd == '-') {
                float vol = max(0.f, music.getVolume() - 10);
                music.setVolume(vol);
                cout << "Volume: " << vol << "%\n";
            } else if (cmd == 'n') {
                showTimer = false;
                music.stop();
                break;
            } else if (cmd == 'b') {
                showTimer = false;
                music.stop();
                return currentIndex - 1;
            } else if (cmd == 's') {
                showTimer = false;
                music.stop();
                cout << "Stopped.\n";
                break;
            } else {
                cout << "Invalid command.\n";
            }
        }

        showTimer = false;
        if (t.joinable()) t.join();  // Wait for timer thread to finish

        return currentIndex + 1;  // Default: go to next
    }

    int main() {
        vector<string> playlist = loadPlaylist("playlist.txt");

        if (playlist.empty()) {
            cout << "Playlist is empty or file not found.\n";
            return 0;
        }

        int currentIndex = -1;

        while (true) {
            if (currentIndex == -1) {
                cout << "\nAvailable Songs:\n";
                for (size_t i = 0; i < playlist.size(); ++i) {
                    cout << i + 1 << ". " << playlist[i] << endl;
                }

                cout << "Enter song number to play (0 to exit): ";
                int choice;
                cin >> choice;
                cin.ignore();

                if (choice == 0) break;

                if (choice < 1 || choice > playlist.size()) {
                    cout << "Invalid choice. Try again.\n";
                    continue;
                }

                currentIndex = choice - 1;
            }

            sf::Music music;
            if (!music.openFromFile(playlist[currentIndex])) {
                cout << "Could not open file: " << playlist[currentIndex] << endl;
                currentIndex = -1;
                continue;
            }

            music.setVolume(75);  // Default volume
            int result = playbackMenu(music, playlist[currentIndex], currentIndex, playlist.size());

            if (result >= 0 && result < playlist.size()) {
                currentIndex = result;
            } else {
                currentIndex = -1;
            }
        }

        cout << "\nGoodbye!\n";
        return 0;
    }
