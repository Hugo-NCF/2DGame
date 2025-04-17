#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>

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
    sf::Texture shootTexture;
public:
    void setShootingAnimation(bool isShooting);
};

#endif