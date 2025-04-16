#include "Player.h"
#include <iostream>

Player::Player() : frameCount(6), currentFrame(0), frameDuration(0.10f), frameTimer(0.0f), speed(300.0f), isRunning(false), facingRight(true) {}

void Player::init() {
    if (!idleTexture.loadFromFile("Assets/Player/Idle.png")) {
        std::cerr << "Error loading idle texture" << std::endl;
        return;
    }
    if (!runTexture.loadFromFile("Assets/Player/Run.png")) {
        std::cerr << "Error loading run texture" << std::endl;
        return;
    }

    playerSprite.setScale(3.0f, 3.0f);
    playerSprite.setPosition(400, 300);

    // Initialize idle animation
    idleFrameRect = sf::IntRect(0, 0, 128, 128);
    runFrameRect = sf::IntRect(0, 0, 128, 128);
    playerSprite.setTexture(idleTexture);
    playerSprite.setTextureRect(idleFrameRect);
}

void Player::update(float deltaTime) {
    // Handle player movement (WASD controls)
    bool moving = false;
    sf::Vector2f currentPosition = playerSprite.getPosition();
    float leftBounds = 128 * 2;
    float rightBounds = 1280 - 128 * 2;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        if (currentPosition.x > leftBounds) {
            playerSprite.move(-speed * deltaTime, 0);
            moving = true;
        }
        if (facingRight) {
            facingRight = false;
            playerSprite.setScale(-3.0f, 3.0f);
            playerSprite.move(128.0f * 3.0f, 0);
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        if (currentPosition.x < rightBounds) {
            playerSprite.move(speed * deltaTime, 0);
            moving = true;
        }
        if (!facingRight) {
            facingRight = true;
            playerSprite.setScale(3.0f, 3.0f);
            playerSprite.move(-128.0f * 3.0f, 0);
        }
    }

   setRunningAnimation(moving);

    // Animate the player character
    frameTimer += deltaTime;
    if (frameTimer >= frameDuration) {
        frameTimer = 0.0f;
        currentFrame = (currentFrame + 1) % frameCount;

        sf::IntRect& currentRect = isRunning ? runFrameRect : idleFrameRect;
        currentRect.left = currentFrame * 128;
        playerSprite.setTextureRect(currentRect);
    }
}

void Player::setRunningAnimation(bool isRunning) {
    if (this->isRunning != isRunning) {
        this->isRunning = isRunning;
        if (isRunning) {
            playerSprite.setTexture(runTexture);
            frameCount = 6;
            frameDuration = 0.08f;
        } else {
            playerSprite.setTexture(idleTexture);
            frameCount = 4;
            frameDuration = 0.15f;
        }
        currentFrame = 0; // Reset to the first frame of the new animation
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(playerSprite);
}

sf::Sprite& Player::getSprite() {
    return playerSprite;
}