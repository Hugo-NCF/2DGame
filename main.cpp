
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include "Player.h"
using namespace std;
using namespace sf;

const float VIEW_WIDTH = 1280.f;
const float VIEW_HEIGHT = 800.f;

int main() {
    // Create the window
    RenderWindow window(VideoMode(VIEW_WIDTH, VIEW_HEIGHT), "Zombie Planet: Crashdown");

    // Load background texture
    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Map/background.png"))
    {
        return -1; // error loading background
    }

    // Create a sprite using the background texture
    Sprite backgroundSprite(backgroundTexture);

    // Scale to fit window
    Vector2u textureSize = backgroundTexture.getSize();
    Vector2u windowSize = window.getSize();

    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;

    backgroundSprite.setScale(scaleX, scaleY);

    Player player;
    player.init();

    // Clock for deltaTime calculation
    sf::Clock clock;

    // Main game loop
    while (window.isOpen()) {
        // Calculate deltaTime
        float deltaTime = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }


        player.update(deltaTime);
                // Clear window
        window.clear();

        // Draw background
        window.draw(backgroundSprite);

        // Draw the player sprite
        player.draw(window);

        // Display everything
        window.display();
    }

    return 0;
}