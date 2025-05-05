#include "Player.h"
#include "Hitbox.h"
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
    isDead(false), // Initialize death state
    hitbox(sf::Vector2f(100.0f, 164.0f)),
    health(100.0f), // Initialize health
    invulnerabilityTimer(0.0f), // Initialize invulnerability timer
    deathAnimationTimer(0.0f) // Initialize death animation timer
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
    if (!deathTexture.loadFromFile("Assets/Player/Dead.png")) { // Load death texture
        std::cerr << "Error loading death texture" << std::endl;
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
    deathFrameRect = sf::IntRect(0, 0, 128, 128); // Initialize death frame rectangle
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
 
    // Load death sound
    if (!deathSoundBuffer.loadFromFile("Assets/playerdeath.mp3")) {
        std::cerr << "Error loading death sound" << std::endl;
    } else {
        deathSound.setBuffer(deathSoundBuffer);
    }

     // Position hitbox at same center
    hitbox.setPosition(playerSprite.getPosition().x, playerSprite.getPosition().y + 128.0f * 3.0f / 2.0f);
}

void Player::update(float deltaTime) {
    if (!isAlive() && !isDead) { // Check if player just died
        setDeathAnimation(true);
        deathAnimationTimer = 0.0f; // Start the death animation timer
    }

    if (isDead) {
        deathAnimationTimer += deltaTime; // Increment death animation timer
        // Update death animation frames
        frameTimer += deltaTime;
        if (frameTimer >= frameDuration) {
            frameTimer = 0.0f;
            currentFrame = (currentFrame + 1); // Don't loop death animation
            if (currentFrame >= frameCount) {
                currentFrame = frameCount - 1; // Hold on last frame
            }
        }
        deathFrameRect.left = currentFrame * 128;
        playerSprite.setTextureRect(deathFrameRect);
        return; // Stop updating other animations and movement if dead
    }

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

    // Update invulnerability timer if active
    if (invulnerabilityTimer > 0) {
        invulnerabilityTimer -= deltaTime;
        
        // Make the sprite flash when invulnerable
        // Make the sprite flash when invulnerable
        if (static_cast<int>(invulnerabilityTimer * 10) % 2 == 0) {
            playerSprite.setColor(sf::Color(255, 255, 255, 128)); // Half transparent
        } else {
            playerSprite.setColor(sf::Color(255, 255, 255, 255)); // Fully visible
        }
    } else {
        playerSprite.setColor(sf::Color(255, 255, 255, 255)); // Ensure full visibility
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
    if (isDead) { // Prioritize death animation
        currentRect = &deathFrameRect;
    }
    else if (isShooting) {
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
    // hitbox.draw(window);
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

void Player::setDeathAnimation(bool isDead) {
    if (this->isDead != isDead) {
        this->isDead = isDead;
        if (isDead) {
            playerSprite.setTexture(deathTexture);
            frameCount = 4; // Assuming 4 frames for death animation
            frameDuration = 0.15f; // Adjust duration as needed
           deathSound.play(); // Play death sound
            currentFrame = 0;
            // Stop other animations
            isRunning = false;
            isShooting = false;
            isReloading = false;
        }
    }
}

void Player::takeDamage(float damage) {
    // Only take damage if not invulnerable
    if (invulnerabilityTimer <= 0) {
        health -= damage;
        if (health < 0) health = 0;
        
        // Set invulnerability for 1 second
        invulnerabilityTimer = 1.0f;
        
        std::cout << "Player took " << damage << " damage. Health: " << health << std::endl;
    }
}

float Player::getHealth() const {
    return health;
}

bool Player::isAlive() const {
    return health > 0;
}

bool Player::isDeathAnimationComplete() const {
    // Assuming death animation has 4 frames and frameDuration is 0.15f
    // The total duration of the death animation is frameCount * frameDuration
    return isDead && deathAnimationTimer >= (frameCount * frameDuration);
}

Hitbox& Player::getHitbox() {
    return hitbox;
}
