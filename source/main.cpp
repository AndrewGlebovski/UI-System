#include <SFML/Graphics.hpp>
#include <stdio.h>
#include "configs.hpp"


int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_W, SCREEN_H), "UI");

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear();
        window.display();
    }

    printf("UI System!\n");
    return 0;
}
