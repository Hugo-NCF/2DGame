#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include "Player.h"
#include "Bullet.h"

#include <sstream>
using namespace std;
using namespace sf;

const float VIEW_WIDTH = 1280.f;
const float VIEW_HEIGHT = 800.f;

int main() {
    // Create the window
    RenderWindow window(VideoMode(VIEW_WIDTH, VIEW_HEIGHT), "Zombie Planet: Crashdown");
    // Load background texture
    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Assets/Backgrounds/background.png"))
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
    
    // Create bullet manager
    BulletManager bulletManager(20);
    
    // Variables for shooting control
    bool canShoot = true;
    bool canReload = true;
    
    // Font for UI
    sf::Font font;
    if (!font.loadFromFile("Assets/pixelFont.ttf")) {
        cout << "Failed to load font!" << endl;
        // Proceed without font
    }
    
    // Ammo display text
    sf::Text ammoText;
    ammoText.setFont(font);
    ammoText.setCharacterSize(24);
    ammoText.setFillColor(sf::Color::White);
    ammoText.setPosition(10, 10);
    
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
                
            // Handle key release events for shooting and reloading
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Space) {
                    canShoot = true;
                }
                if (event.key.code == sf::Keyboard::R) {
                    canReload = true;
                }
            }
        }
        
        player.update(deltaTime);
        
        // Handle shooting
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && canShoot) {
            // Get player position and direction
            float bulletX = player.isFacingRight() ?
                player.getPosition().x + player.getSize().x - 10.0f :
                player.getPosition().x - player.getSize().x + 10.0f;
            float bulletY = player.getPosition().y + player.getSize().y / 1.5;

            bulletManager.fireBullet(bulletX, bulletY, player.isFacingRight());
            canShoot = false;
        }
        
        // Handle reload
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && canReload) {
            bulletManager.reload();
            canReload = false;
        }
        
        // Update bullets
        bulletManager.update(deltaTime);
        
        // Update ammo display
        std::stringstream ss;
        ss << "Ammo: " << bulletManager.getRemainingBullets() << " / 20";
        ammoText.setString(ss.str());
        
        // Clear window
        window.clear();
        // Draw background
        window.draw(backgroundSprite);
        // Draw the player sprite
        player.draw(window);
        // Draw bullets
        bulletManager.render(window);
        // Draw UI
        window.draw(ammoText);
        // Display everything
        window.display();
    }
    return 0;
}
