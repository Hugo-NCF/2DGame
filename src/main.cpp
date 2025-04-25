#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Audio.hpp>
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"

#include <sstream>
using namespace std;
using namespace sf;

const float VIEW_WIDTH = 1280.f;
const float VIEW_HEIGHT = 800.f;

// Function to check collision between bullets and enemies
void checkBulletEnemyCollisions(BulletManager& bulletManager, EnemyManager& enemyManager) {
    Bullet* bullets = bulletManager.getBullets();
    int maxBullets = bulletManager.getMaxBullets();
    
    for (int i = 0; i < maxBullets; i++) {
        if (bullets[i].isActive()) {
            for (auto& enemy : enemyManager.getEnemies()) {
                if (enemy->isAlive() && enemy->getHitbox() && 
                    enemy->getHitbox()->checkCollision(bullets[i].getBounds())) {
                    // Apply damage to enemy
                    enemy->takeDamage(bullets[i].getDamage());
                    
                    // Deactivate bullet
                    bullets[i].setActive(false);
                    
                    break; // Break to avoid a single bullet hitting multiple enemies
                }
            }
        }
    }
}

// Function to check collision between player and enemies
void checkPlayerEnemyCollisions(Player& player, EnemyManager& enemyManager) {
    for (auto& enemy : enemyManager.getEnemies()) {
        if (enemy->isAlive() && enemy->getHitbox() && 
            enemy->getHitbox()->checkCollision(player.getHitbox().getBounds())) {
            
            // Only take damage if enemy is attacking
            if (enemy->isAttacking()) {
                player.takeDamage(enemy->getAttackDamage());
            }
        }
    }
}

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
    
    // Load and play background music
    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("Assets/Damned.mp3")) {
        std::cerr << "Error loading background music" << std::endl;
        // Handle error or continue without music
    } else {
        backgroundMusic.setLoop(true); // Set music to loop
        backgroundMusic.play(); // Start playing music
    }
    
    // Create bullet manager and enemy manager
    BulletManager bulletManager(20);
    EnemyManager enemyManager;
    
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
    
    // Health display text
    sf::Text healthText;
    healthText.setFont(font);
    healthText.setCharacterSize(24);
    healthText.setFillColor(sf::Color::White);
    healthText.setPosition(10, 50);
    
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && canShoot && !player.getIsReloading()) {
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
        
        // Update bullets and enemies
        bulletManager.update(deltaTime);
        enemyManager.update(deltaTime, player.getPosition());
        
        // Check for collisions
        checkBulletEnemyCollisions(bulletManager, enemyManager);
        checkPlayerEnemyCollisions(player, enemyManager);
        
        // Update ammo display
        std::stringstream ss;
        ss << "Ammo: " << bulletManager.getRemainingBullets() << " / 20";
        ammoText.setString(ss.str());
        
        // Update health display
        std::stringstream healthSS;
        healthSS << "Health: " << static_cast<int>(player.getHealth());
        healthText.setString(healthSS.str());
        
        // Check if player is dead
        if (!player.isAlive()) {
            std::cout << "Game Over!" << std::endl;
            window.close();
        }
        
        // Clear window
        window.clear();
        // Draw background
        window.draw(backgroundSprite);
        // Draw the player sprite
        player.draw(window);
        // Draw bullets
        bulletManager.render(window);
        //Draw the enemy
        enemyManager.render(window);
        // Draw UI
        window.draw(ammoText);
        window.draw(healthText);
        // Display everything
        window.display();
    }
    return 0;
}
