#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Player {
public:
    Player();
    void init();
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    sf::Sprite& getSprite();
    void setRunningAnimation(bool isRunning);
    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;
    bool isFacingRight() const;
private:
    sf::Texture idleTexture;
    sf::Texture runTexture;
    sf::Sprite playerSprite;
    int frameCount;
    int currentFrame;
    float frameDuration;
    float frameTimer;
    bool isRunning;
    bool facingRight;
    sf::IntRect idleFrameRect;
    sf::IntRect runFrameRect;
    sf::IntRect shootFrameRect;
    float speed;
    bool isShooting;
    float shootingTimer;
    bool isReloading;
    float reloadingTimer;
    sf::Texture shootTexture;
    sf::Texture reloadTexture;
    sf::IntRect reloadFrameRect;
    bool reloadKeyPressed;

    sf::SoundBuffer shootBuffer;
    sf::Sound shootSound;
    sf::SoundBuffer reloadBuffer;
    sf::Sound reloadSound;
public:
    void setShootingAnimation(bool isShooting);
    void setReloadingAnimation(bool isReloading);
    bool getIsReloading() const; // Add getter for isReloading
};

#endif