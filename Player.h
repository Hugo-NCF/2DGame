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
    sf::IntRect frameRect;
    float speed;
};

#endif