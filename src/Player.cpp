#include "Player.h"
#include <iostream>

Player::Player() : frameCount(6), currentFrame(0), frameDuration(0.10f), frameTimer(0.0f), speed(300.0f), isRunning(false), facingRight(true), isShooting(false), shootingTimer(0.0f) {}

void Player::init() {
    if (!idleTexture.loadFromFile("Assets/Player/Idle.png")) {
        std::cerr << "Error loading idle texture" << std::endl;
        return;
    }
    if (!runTexture.loadFromFile("Assets/Player/Run.png")) {
        std::cerr << "Error loading run texture" << std::endl;
        return;
    }
    if (!shootTexture.loadFromFile("Assets/Player/Shot_2.png")) {
        std::cerr << "Error loading shoot texture" << std::endl;
        return;
    }

    playerSprite.setScale(3.0f, 3.0f);
    playerSprite.setPosition(400, 300);

    // Initialize idle animation
    idleFrameRect = sf::IntRect(0, 0, 128, 128);
    runFrameRect = sf::IntRect(0, 0, 128, 128);
    shootFrameRect = sf::IntRect(0, 0, 128, 128);
    playerSprite.setTexture(idleTexture);
    playerSprite.setTextureRect(idleFrameRect);
}

void Player::update(float deltaTime) {
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

    setRunningAnimation(moving); // Update running state first

    // Handle shooting key
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        setShootingAnimation(true);
    }

    // Stop shooting after timer
    if (isShooting) {
        shootingTimer += deltaTime;
        if (shootingTimer > 0.2f) {
            shootingTimer = 0.0f;
            setShootingAnimation(false);  // Will fallback to run/idle depending on state
        }
    }

    // Animate
    frameTimer += deltaTime;
    if (frameTimer >= frameDuration) {
        frameTimer = 0.0f;
        currentFrame = (currentFrame + 1) % frameCount;
    }

    // Decide which frameRect to use
    sf::IntRect* currentRect;
    if (isShooting) {
        currentRect = &shootFrameRect;
    }
    else if (isRunning) {
        currentRect = &runFrameRect;
    }
    else {
        currentRect = &idleFrameRect;
    }

    currentRect->left = currentFrame * 128;
    playerSprite.setTextureRect(*currentRect);
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
    } else if (isShooting) {
        playerSprite.setTexture(shootTexture);
        frameCount = 2;
        frameDuration = 0.1f;
        currentFrame = 0;
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(playerSprite);
}

sf::Sprite& Player::getSprite() {
    return playerSprite;
}

sf::Vector2f Player::getPosition() const {
    return playerSprite.getPosition();
}

sf::Vector2f Player::getSize() const {
    return sf::Vector2f(playerSprite.getGlobalBounds().width, playerSprite.getGlobalBounds().height);
}

bool Player::isFacingRight() const {
    return facingRight;
}

void Player::setShootingAnimation(bool isShooting) {
    if (this->isShooting != isShooting) {
        this->isShooting = isShooting;
        if (isShooting) {
            playerSprite.setTexture(shootTexture);
            frameCount = 2;
            frameDuration = 0.1f;
        }
        else {
            // Restore correct texture based on running state
            if (isRunning) {
                playerSprite.setTexture(runTexture);
                frameCount = 6;
                frameDuration = 0.08f;
            }
            else {
                playerSprite.setTexture(idleTexture);
                frameCount = 4;
                frameDuration = 0.15f;
            }
        }
        currentFrame = 0; // Reset to the first frame of the new animation
    }
}
