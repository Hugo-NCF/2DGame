#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Hitbox.h"

class Player {
public:
    Player();

    void init();
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    sf::Sprite& getSprite();
    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;
    bool isFacingRight() const;
    bool getIsReloading() const; // Added missing declaration

    // New methods for health and combat
    Hitbox& getHitbox(); // Added missing declaration
    void takeDamage(float damage);
    float getHealth() const;
    bool isAlive() const;
    void setDeathAnimation(bool isDead); // Added death animation method
    bool isDeathAnimationComplete() const; // Added method to check if death animation is complete

private:
    void setRunningAnimation(bool isRunning);
    void setShootingAnimation(bool isShooting);
    void setReloadingAnimation(bool isReloading);

    sf::Sprite playerSprite;
    sf::Texture idleTexture;
    sf::Texture runTexture;
    sf::Texture shootTexture;
    sf::Texture reloadTexture;
    sf::Texture deathTexture; // Added death texture

    sf::IntRect idleFrameRect;
    sf::IntRect runFrameRect;
    sf::IntRect shootFrameRect;
    sf::IntRect reloadFrameRect;
    sf::IntRect deathFrameRect; // Added death frame rectangle

    Hitbox hitbox;  // Custom hitbox for the player
    sf::SoundBuffer shootSoundBuffer;
    sf::Sound shootSound;
    sf::SoundBuffer reloadSoundBuffer;
    sf::Sound reloadSound;
    sf::SoundBuffer deathSoundBuffer; // Added death sound buffer
    sf::Sound deathSound; // Added death sound

    int frameCount;
    int currentFrame;
    float frameDuration;
    float frameTimer;

    float speed;
    bool isRunning;
    bool facingRight;
    bool isShooting;
    bool isReloading;
    bool reloadKeyPressed;
    float shootingTimer;
    float reloadingTimer;
    bool isDead; // Added flag for death state

    // New member variables for health and invulnerability
    float health;
    float invulnerabilityTimer;
    float deathAnimationTimer; // Added timer for death animation delay
};

#endif
