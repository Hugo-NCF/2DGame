#include "Enemy.h"
#include "Hitbox.h"
#include <iostream>
#include <cmath>
#include <random>

Enemy::Enemy() :
    currentState(IDLE),
    facingRight(false),
    speed(100.0f),
    health(30.0f),
    attackDamage(5.0f),
    attackRange(50.0f),
    detectionRange(1000.0f),
    frameWidth(128),
    frameHeight(128),
    currentFrame(0),
    totalFrames(0),
    animationTimer(0.0f),
    frameDuration(0.1f),
    attackCooldown(1.0f),
    attackTimer(0.0f),
    isDeathAnimationFinished(false),
    deathRemoveTimer(0.0f),
    deathRemoveDelay(0.5f), // 0.5 second delay after death animation
    
    hitbox(nullptr)
{
}

Enemy::~Enemy() {
    if (hitbox) {
        delete hitbox;
        hitbox = nullptr;
    }
}

void Enemy::loadTextures() {
    std::string texturePath;
    switch (enemyType) {
        case ZOMBIE_1:
            texturePath = "Assets/Zombie/";
            break;
        case ZOMBIE_2:
            texturePath = "Assets/Zombie_2/";
            break;
        default:
            std::cerr << "Unknown enemy type!" << std::endl;
            return;
    }

    if (!idleTexture.loadFromFile(texturePath + "Idle.png")) {
        std::cerr << "Failed to load idle texture for enemy type " << enemyType << "!" << std::endl;
    }

    if (!walkTexture.loadFromFile(texturePath + "Walk.png")) {
        std::cerr << "Failed to load walk texture for enemy type " << enemyType << "!" << std::endl;
    }

    if (!attackTexture.loadFromFile(texturePath + "Attack.png")) {
        std::cerr << "Failed to load attack texture for enemy type " << enemyType << "!" << std::endl;
    }

    if (!deadTexture.loadFromFile(texturePath + "Dead.png")) {
        std::cerr << "Failed to load dead texture for enemy type " << enemyType << "!" << std::endl;
    }
}

void Enemy::initSprite() {
    sprite.setTexture(idleTexture);

    // Set origin to center bottom
    sprite.setOrigin(frameWidth / 2.0f, frameHeight);

    // Set initial frame
    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));

    // Apply position
    sprite.setPosition(position);

    // Initial scale
    // Apply scale
    sprite.setScale(3.0f, 3.0f);
    // Adjust x-scale for horizontal flipping
    if (!facingRight) {
        sprite.setScale(-3.0f, 3.0f);
    }
}

void Enemy::init(EnemyType type, float startX, float playerY, bool spawnOnRight) {
    enemyType = type; // Store the enemy type

    // Set different speed for each type
    switch (enemyType) {
    case ZOMBIE_1:
        speed = 100.0f;
        break;
    case ZOMBIE_2:
        speed = 150.0f; // Faster zombie
        break;
    default:
        speed = 100.0f;
        break;
    }

    // Calculate initial position with y-offset to align feet with player
    position = sf::Vector2f(startX, playerY + frameHeight * 3.0f / 2.0f);
    facingRight = !spawnOnRight; // Face toward center

    loadTextures();
    initSprite();

    // Load death sound
    if (!deathSoundBuffer.loadFromFile("Assets/zombiedeath.mp3")) {
        std::cerr << "Error loading zombie death sound" << std::endl;
    } else {
        deathSound.setBuffer(deathSoundBuffer);
    }
 
     // Create hitbox
     hitbox = new Hitbox(sf::Vector2f(100.0f, 164.0f)); // Same size as player hitbox
     hitbox->setPosition(position.x, position.y - 100.0f);

    // Initialize animation for idle
    currentState = IDLE;
    totalFrames = 15; // Idle animation frames (adjust based on your spritesheet)
    currentFrame = 0;
    animationTimer = 0.0f;
}

void Enemy::update(float deltaTime, const sf::Vector2f& playerPosition) {
    if (!isAlive()) {
        // Handle death animation
        if (currentState != DEAD) {
            currentState = DEAD;
            currentFrame = 0;
            animationTimer = 0.0f;
            totalFrames = 12; // Death animation frames
        }
    }
    else {
        // Update state based on player position
        updateState(playerPosition);

        // Move enemy based on state
        if (currentState == WALKING) {
            float moveDirection = facingRight ? 1.0f : -1.0f;
            position.x += speed * moveDirection * deltaTime;
        }

        // Update facing direction based on player position
        bool shouldFaceRight = playerPosition.x > position.x;
        if (shouldFaceRight != facingRight) {
            facingRight = shouldFaceRight;
            sprite.setScale(facingRight ? 1.0f : -1.0f, 1.0f);
        }

        // Update attack timer
        if (attackTimer > 0) {
            attackTimer -= deltaTime;
        }
    }

    // Update animation
    updateAnimation(deltaTime);

    // Update death remove timer if death animation is finished
    if (isDeathAnimationFinished) {
        deathRemoveTimer += deltaTime;
    }

    // Update sprite position
    sprite.setPosition(position);

    // Update hitbox position
    if (hitbox) {
        hitbox->setPosition(position.x, position.y - 100.0f);
    }
}

void Enemy::updateState(const sf::Vector2f& playerPosition) {
    float distanceToPlayer = std::abs(playerPosition.x - position.x);

    if (distanceToPlayer <= attackRange) {
        // Player is in attack range
        if (attackTimer <= 0) {
            currentState = ATTACKING;
            attackTimer = attackCooldown;
            currentFrame = 0; // Reset animation frame
            animationTimer = 0.0f; // Reset animation timer
            totalFrames = 8; // Attack animation frames
            frameDuration = 0.1f; // Default duration for attack animation
            sprite.setTexture(attackTexture); // Set texture immediately
            sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight)); // Set initial frame
            // Reapply scale
            sprite.setScale(3.0f, 3.0f);
            if (!facingRight) {
                sprite.setScale(-3.0f, 3.0f);
            }
        }
        else if (currentState != ATTACKING) {
            currentState = IDLE;
            currentFrame = 0; // Reset animation frame
            animationTimer = 0.0f; // Reset animation timer
            frameDuration = 0.1f; // Default duration for idle
            sprite.setTexture(idleTexture); // Set texture immediately
            sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight)); // Set initial frame
            // Reapply scale
            sprite.setScale(3.0f, 3.0f);
            if (!facingRight) {
                sprite.setScale(-3.0f, 3.0f);
            }
        }
    }
    else if (distanceToPlayer <= detectionRange) {
        // Player is detected but not in attack range
        if (currentState != WALKING) { // Only reset if changing to WALKING
            currentState = WALKING;
            currentFrame = 0; // Reset animation frame
            animationTimer = 0.0f; // Reset animation timer
            totalFrames = 10; // Walk animation frames
            frameDuration = 0.1f; // Default duration for walking
            sprite.setTexture(walkTexture); // Set texture immediately
            sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight)); // Set initial frame
            // Reapply scale
            sprite.setScale(3.0f, 3.0f);
            if (!facingRight) {
                sprite.setScale(-3.0f, 3.0f);
            }
        }
    }
    else {
        // Player is not detected
        if (currentState != IDLE) { // Only reset if changing to IDLE
            currentState = IDLE;
            currentFrame = 0; // Reset animation frame
            animationTimer = 0.0f; // Reset animation timer
            totalFrames = 15; // Idle animation frames
            frameDuration = 0.1f; // Default duration for idle
            sprite.setTexture(idleTexture); // Set texture immediately
            sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight)); // Set initial frame
            // Reapply scale
            sprite.setScale(3.0f, 3.0f);
            if (!facingRight) {
                sprite.setScale(-3.0f, 3.0f);
            }
        }
    }
}

void Enemy::updateAnimation(float deltaTime) {
    animationTimer += deltaTime;

    if (animationTimer >= frameDuration) {
        // Move to next frame
        currentFrame = (currentFrame + 1) % totalFrames;
        animationTimer = 0.0f;

        // Check if death animation is complete
        if (currentState == DEAD && currentFrame >= totalFrames - 1) {
            isDeathAnimationFinished = true;
            currentFrame = totalFrames - 1; // Stay on last frame
        }

        // Update texture based on state
        sf::Texture* currentTexture = nullptr;

        switch (currentState) {
        case IDLE:
            currentTexture = &idleTexture;
            break;
        case WALKING:
            currentTexture = &walkTexture;
            break;
        case ATTACKING:
            currentTexture = &attackTexture;
            break;
        case DEAD:
            currentTexture = &deadTexture;
            break;
        }

        if (currentTexture) {
            sprite.setTexture(*currentTexture);
            sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
            // Reapply scale after changing texture
            sprite.setScale(3.0f, 3.0f);
            if (!facingRight) {
                sprite.setScale(-3.0f, 3.0f);
            }
        }
    }
}

void Enemy::draw(sf::RenderWindow& window) {
    window.draw(sprite);

    // Debug: Draw hitbox
    // if (hitbox) {
    //     hitbox->draw(window);
    // }
}

void Enemy::takeDamage(float damage) {
    if (isAlive()) {
        health -= damage;
        if (health <= 0) {
            health = 0;
            currentState = DEAD;
            currentFrame = 0;
            animationTimer = 0.0f;
            deathSound.play(); // Play death sound
        }
    }
}

bool Enemy::isAlive() const {
    return health > 0;
}

bool Enemy::canBeRemoved() const {
    return isDeathAnimationFinished;
}

sf::Vector2f Enemy::getPosition() const {
    return position;
}

sf::FloatRect Enemy::getBounds() const {
    return sprite.getGlobalBounds();
}

float Enemy::getAttackDamage() const {
    return attackDamage;
}

Hitbox* Enemy::getHitbox() const {
    return hitbox;
}

bool Enemy::isAttacking() const {
    return currentState == ATTACKING;
}

//------------------------------------------------------------------------------
// EnemyManager Implementation
//------------------------------------------------------------------------------

EnemyManager::EnemyManager() :
    spawnTimer(0.0f),
    spawnInterval(2.0f),
    maxEnemies(20),
    waveDelayTimer(0.0f),
    waveDelay(3.0f),
    isWaveTransitioning(false),
    currentWaveNumber(1),
    enemiesKilledThisWave(0)
{}

EnemyManager::~EnemyManager() {
    for (std::vector<Enemy*>::iterator it = enemies.begin(); it != enemies.end(); ++it) {
        delete *it;
    }
    enemies.clear();
}

void EnemyManager::init() {
    currentWaveNumber = 1;
    spawnInterval = 2.0f;

    // Load font for wave transition text
    if (!waveFont.loadFromFile("Assets/pixelFont.ttf")) { // Replace with your actual font path
        std::cerr << "Failed to load font!" << std::endl;
    }

    // Set up wave transition text properties
    waveTransitionText.setFont(waveFont);
    waveTransitionText.setCharacterSize(50);  // Font size
    waveTransitionText.setFillColor(sf::Color::White);  // Text color
    waveTransitionText.setString("Wave 1"); // Initial wave text
    waveTransitionText.setPosition(640, 360);  // Centered position on screen
}


void EnemyManager::update(float deltaTime, const sf::Vector2f& playerPosition) {
    // If wave transition is in progress, update the delay timer
    if (isWaveTransitioning) {
        waveDelayTimer += deltaTime;

        // Set wave transition text based on the current wave
        waveTransitionText.setString("Wave " + std::to_string(currentWaveNumber));

        if (waveDelayTimer >= waveDelay) {
            // End wave transition, allow spawning enemies
            isWaveTransitioning = false;
            waveDelayTimer = 0.0f; // Reset the timer
        }
        return; // Skip further updates if wave transition is in progress
    }

    // Update existing enemies
    for (auto& enemy : enemies) {
        if (enemy) {
            enemy->update(deltaTime, playerPosition);
        }
    }

    // Handle enemy spawning
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval && static_cast<int>(enemies.size()) < maxEnemies) {
        spawnEnemy(playerPosition);
        spawnTimer = 0.0f;
    }

    // Remove dead enemies
    removeDeadEnemies();

    // Checks for wave increase
    if (enemiesKilledThisWave >= 10 * currentWaveNumber) {
        increaseWave();
        enemiesKilledThisWave = 0;
        isWaveTransitioning = true; // Start wave transition
    }
}

void EnemyManager::render(sf::RenderWindow& window) {
    for (std::vector<Enemy*>::iterator it = enemies.begin(); it != enemies.end(); ++it) {
        if (*it) {
            (*it)->draw(window);
        }
    }

    // If wave transition is in progress, draw wave transition text
    if (isWaveTransitioning) {
        std::cout << "Drawing transition text\n";
        window.draw(waveTransitionText);
    }
}

void EnemyManager::spawnEnemy(const sf::Vector2f& playerPosition) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sideDist(0, 1); // 0 for left, 1 for right
    std::uniform_real_distribution<float> offsetDist(100.0f, 300.0f);

    // Determine which enemy types to spawn based on current wave
    std::uniform_int_distribution<> typeDist(0, (currentWaveNumber > 1 ? 1 : 0));

    // Decide which side to spawn on
    bool spawnOnRight = sideDist(gen) == 1;

    // Calculate spawn position
    float spawnX;
    if (spawnOnRight) {
        spawnX = playerPosition.x + 1280.0f / 2.0f + offsetDist(gen); // Right side of screen
    }
    else {
        spawnX = playerPosition.x - 1280.0f / 2.0f - offsetDist(gen); // Left side of screen
    }

    // Randomly select enemy type based on current wave
    EnemyType enemyType = static_cast<EnemyType>(typeDist(gen));

    // Create and initialize new enemy
    Enemy* newEnemy = new Enemy();
    newEnemy->init(enemyType, spawnX, playerPosition.y, spawnOnRight);

    enemies.push_back(newEnemy);
}

void EnemyManager::removeDeadEnemies() {
    for (size_t i = 0; i < enemies.size(); /* no increment */) {
        if (enemies[i]->canBeRemoved()) {
            if (!enemies[i]->isAlive()) {
                enemiesKilledThisWave++;
            }
            delete enemies[i];
            enemies.erase(enemies.begin() + i);
        }
        else {
            ++i;
        }
    }
}

void EnemyManager::increaseWave() {
    currentWaveNumber++;
    // Make spawning faster as waves progress
    spawnInterval = std::max(0.5f, spawnInterval * 0.5f);

    isWaveTransitioning = true;
    waveDelayTimer = 0.0f;  // Reset the timer for the next transition

    for (Enemy* enemy : enemies) {
        if (enemy && enemy->isAlive()) {
            enemy->takeDamage(9999.0f);
        }
    }
}


int EnemyManager::getCurrentWave() const {
    return currentWaveNumber;
}

int EnemyManager::getRemainingEnemies() const {
    // Count alive enemies
    int count = 0;
    for (std::vector<Enemy*>::const_iterator it = enemies.begin(); it != enemies.end(); ++it) {
        if (*it && (*it)->isAlive()) {
            count++;
        }
    }
    return count;
}

std::vector<Enemy*>& EnemyManager::getEnemies() {
    return enemies;
}

bool EnemyManager::isWaveCleared() const {
    return enemiesKilledThisWave >= 10 * currentWaveNumber;
}
