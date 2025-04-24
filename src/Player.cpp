#include "Player.h"
#include "Hitbox.h" // Include Hitbox class
#include <iostream>

Player::Player() :
    frameCount(6),
    currentFrame(0),
    frameDuration(0.10f),
    frameTimer(0.0f),
    speed(300.0f),
    isRunning(false),
    facingRight(true),
    isShooting(false),
    shootingTimer(0.0f),
    isReloading(false),
    reloadingTimer(0.0f),
    reloadKeyPressed(false),
    hitbox(sf::Vector2f(100.0f, 164.0f))  // Initialize the hitbox with a custom size (e.g., 100x128)
{
}

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
    if (!reloadTexture.loadFromFile("Assets/Player/Recharge.png")) {
        std::cerr << "Error loading reload texture" << std::endl;
        return;
    }

    // Set the origin to center of 128x128 texture before scaling
    playerSprite.setOrigin(64.f, 64.f); // Center of the sprite (128 / 2)

    // Scale and position the sprite
    playerSprite.setScale(3.0f, 3.0f);
    playerSprite.setPosition(400, 300 + 128.0f * 3.0f / 2.0f); // This sets his feet to the ground

    // Initialize animation frame rectangles
    idleFrameRect = sf::IntRect(0, 0, 128, 128);
    runFrameRect = sf::IntRect(0, 0, 128, 128);
    shootFrameRect = sf::IntRect(0, 0, 128, 128);
    reloadFrameRect = sf::IntRect(0, 0, 128, 128);
    playerSprite.setTexture(idleTexture);
    playerSprite.setTextureRect(idleFrameRect);

    // Load sounds
    if (!shootSoundBuffer.loadFromFile("Assets/shoot.mp3")) {
        std::cerr << "Error loading shoot sound" << std::endl;
    } else {
        shootSound.setBuffer(shootSoundBuffer);
    }

    if (!reloadSoundBuffer.loadFromFile("Assets/reload.mp3")) {
        std::cerr << "Error loading reload sound" << std::endl;
    } else {
        reloadSound.setBuffer(reloadSoundBuffer);
    }

    // Position hitbox at same center
    hitbox.setPosition(playerSprite.getPosition().x, playerSprite.getPosition().y + 128.0f * 3.0f / 2.0f);

}

void Player::update(float deltaTime) {
    bool moving = false;
    sf::Vector2f currentPosition = playerSprite.getPosition();
    float leftBounds = 100; // Expanded left barrier
    float rightBounds = 1280 - 100; // Expanded right barrier

    // Moving logic for A and D keys
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        if (currentPosition.x > leftBounds) {
            playerSprite.move(-speed * deltaTime, 0);
            moving = true;
        }
        if (facingRight) {
            facingRight = false;
            playerSprite.setScale(-3.0f, 3.0f);  // Flip the sprite without moving it
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        if (currentPosition.x < rightBounds) {
            playerSprite.move(speed * deltaTime, 0);
            moving = true;
        }
        if (!facingRight) {
            facingRight = true;
            playerSprite.setScale(3.0f, 3.0f);  // Flip the sprite without moving it
        }
    }

    setRunningAnimation(moving);

    // Handle shooting key
    if (isReloading) {
        // Do nothing if reloading
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        if (!isShooting) {
            setShootingAnimation(true);
            shootSound.play();
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && !reloadKeyPressed && !isReloading) {
        setReloadingAnimation(true);
        reloadKeyPressed = true;
        reloadSound.play();
    }
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        reloadKeyPressed = false;
    }

    // Stop shooting after timer
    if (isShooting) {
        shootingTimer += deltaTime;
        if (shootingTimer > 0.2f) {
            shootingTimer = 0.0f;
            setShootingAnimation(false);
        }
    }

    if (isReloading) {
        if (reloadSound.getStatus() == sf::Sound::Stopped) {
            setReloadingAnimation(false);
        }
    }

    // Animate
    frameTimer += deltaTime;
    if (frameTimer >= frameDuration) {
        frameTimer = 0.0f;
        currentFrame = (currentFrame + 1) % frameCount;
    }

    // Update hitbox position to match sprite's position + adjusted for sprite height
    hitbox.setPosition(playerSprite.getPosition().x, playerSprite.getPosition().y + playerSprite.getLocalBounds().height / 2.0f);

    // Decide which frameRect to use
    sf::IntRect* currentRect;
    if (isShooting) {
        currentRect = &shootFrameRect;
    }
    else if (isReloading) {
        currentRect = &reloadFrameRect;
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
        }
        else {
            playerSprite.setTexture(idleTexture);
            frameCount = 4;
            frameDuration = 0.15f;
        }
        currentFrame = 0;
    }
    else if (isShooting) {
        return;
    }
    else if (isReloading) {
        return;
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(playerSprite);

    // Draw hitbox for debugging
    hitbox.draw(window);
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

bool Player::getIsReloading() const {
    return isReloading;
}

void Player::setShootingAnimation(bool isShooting) {
    if (isReloading) {
        return;
    }
    if (this->isShooting != isShooting) {
        this->isShooting = isShooting;
        if (isShooting) {
            playerSprite.setTexture(shootTexture);
            frameCount = 2;
            frameDuration = 0.1f;
        }
        else {
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
        currentFrame = 0;
    }
}

void Player::setReloadingAnimation(bool isReloading) {
    if (this->isReloading != isReloading) {
        this->isReloading = isReloading;
        if (isReloading) {
            playerSprite.setTexture(reloadTexture);
            frameCount = 4;
            frameDuration = 0.1f;
        }
        else {
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
        currentFrame = 0;
    }
}