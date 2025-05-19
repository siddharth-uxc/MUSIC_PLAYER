#include <SFML/Audio.hpp>
#include <iostream>

using namespace std;

int main() {
    sf::Music music;
    if (!music.openFromFile("countingstars.ogg")) {
        cout << "Could not open file: countingstars.ogg\n";
        return -1;
    }

    music.play();
    cout << "Playing song. Press Enter to stop...\n";
    cin.get();

    music.stop();
    return 0;
}
