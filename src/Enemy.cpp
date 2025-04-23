#include "Enemy.h"
#include "Hitbox.h"
#include <iostream>
#include <cmath>
#include <random>

Enemy::Enemy() :
    currentState(EnemyState::IDLE),
    facingRight(false),
    speed(100.0f),
    health(100.0f),
    attackDamage(10.0f),
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
    if (!idleTexture.loadFromFile("Assets/Zombie/Idle.png")) {
        std::cerr << "Failed to load zombie idle texture!" << std::endl;
    }
    
    if (!walkTexture.loadFromFile("Assets/Zombie/Walk.png")) {
        std::cerr << "Failed to load zombie walk texture!" << std::endl;
    }
    
    if (!attackTexture.loadFromFile("Assets/Zombie/Attack.png")) {
        std::cerr << "Failed to load zombie attack texture!" << std::endl;
    }
    
    if (!deadTexture.loadFromFile("Assets/Zombie/Dead.png")) {
        std::cerr << "Failed to load zombie dead texture!" << std::endl;
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
    sprite.setScale(1.0f, 1.0f);
    if (!facingRight) {
        sprite.setScale(3.0f, 3.0f);
    }
}

void Enemy::init(float startX, bool spawnOnRight) {
    position = sf::Vector2f(startX, 600.0f); // Ground level
    facingRight = !spawnOnRight; // Face toward center
    
    loadTextures();
    initSprite();
    
    // Create hitbox
    hitbox = new Hitbox(sf::Vector2f(40.0f, 100.0f)); // Smaller than the sprite
    hitbox->setPosition(position.x, position.y - 50.0f);
    
    // Initialize animation for idle
    currentState = EnemyState::IDLE;
    totalFrames = 15; // Idle animation frames (adjust based on your spritesheet)
    currentFrame = 0;
    animationTimer = 0.0f;
}

void Enemy::update(float deltaTime, const sf::Vector2f& playerPosition) {
    if (!isAlive()) {
        // Handle death animation
        if (currentState != EnemyState::DEAD) {
            currentState = EnemyState::DEAD;
            currentFrame = 0;
            animationTimer = 0.0f;
            totalFrames = 12; // Death animation frames
        }
    } else {
        // Update state based on player position
        updateState(playerPosition);
        
        // Move enemy based on state
        if (currentState == EnemyState::WALKING) {
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
    
    // Update sprite position
    sprite.setPosition(position);
    
    // Update hitbox position
    if (hitbox) {
        hitbox->setPosition(position.x, position.y - 50.0f);
    }
}

void Enemy::updateState(const sf::Vector2f& playerPosition) {
    float distanceToPlayer = std::abs(playerPosition.x - position.x);
    
    if (distanceToPlayer <= attackRange) {
        // Player is in attack range
        if (attackTimer <= 0) {
            currentState = EnemyState::ATTACKING;
            attackTimer = attackCooldown;
            
            // Reset animation for attack
            currentFrame = 0;
            animationTimer = 0.0f;
            totalFrames = 8; // Attack animation frames
        } else if (currentState != EnemyState::ATTACKING) {
            currentState = EnemyState::IDLE;
        }
    } else if (distanceToPlayer <= detectionRange) {
        // Player is detected but not in attack range
        currentState = EnemyState::WALKING;
        totalFrames = 10; // Walk animation frames
    } else {
        // Player is not detected
        currentState = EnemyState::IDLE;
        totalFrames = 15; // Idle animation frames
    }
}

void Enemy::updateAnimation(float deltaTime) {
    animationTimer += deltaTime;
    
    if (animationTimer >= frameDuration) {
        // Move to next frame
        currentFrame = (currentFrame + 1) % totalFrames;
        animationTimer = 0.0f;
        
        // Check if death animation is complete
        if (currentState == EnemyState::DEAD && currentFrame >= totalFrames - 1) {
            isDeathAnimationFinished = true;
            currentFrame = totalFrames - 1; // Stay on last frame
        }
        
        // Update texture based on state
        sf::Texture* currentTexture = nullptr;
        
        switch (currentState) {
            case EnemyState::IDLE:
                currentTexture = &idleTexture;
                break;
            case EnemyState::WALKING:
                currentTexture = &walkTexture;
                break;
            case EnemyState::ATTACKING:
                currentTexture = &attackTexture;
                break;
            case EnemyState::DEAD:
                currentTexture = &deadTexture;
                break;
        }
        
        if (currentTexture) {
            sprite.setTexture(*currentTexture);
            sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
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
            currentState = EnemyState::DEAD;
            currentFrame = 0;
            animationTimer = 0.0f;
        }
    }
}

bool Enemy::isAlive() const {
    return health > 0;
}

bool Enemy::canBeRemoved() const {
    return !isAlive() && isDeathAnimationFinished;
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
    return currentState == EnemyState::ATTACKING;
}

//------------------------------------------------------------------------------
// EnemyManager Implementation
//------------------------------------------------------------------------------

EnemyManager::EnemyManager() :
    spawnTimer(0.0f),
    spawnInterval(2.0f),
    maxEnemies(20),
    currentWaveNumber(1),
    enemiesPerWave(5)
{
}

EnemyManager::~EnemyManager() {
    for (auto enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
}

void EnemyManager::init(int enemiesPerWave) {
    this->enemiesPerWave = enemiesPerWave;
    currentWaveNumber = 1;
    spawnInterval = 2.0f;
}

void EnemyManager::update(float deltaTime, const sf::Vector2f& playerPosition) {
    // Update existing enemies
    for (auto& enemy : enemies) {
        if (enemy) {
            enemy->update(deltaTime, playerPosition);
        }
    }
    
    // Handle enemy spawning
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval && 
        static_cast<int>(enemies.size()) < enemiesPerWave * currentWaveNumber && 
        static_cast<int>(enemies.size()) < maxEnemies) {
        
        spawnEnemy(playerPosition.x);
        spawnTimer = 0.0f;
    }
    
    // Remove dead enemies
    removeDeadEnemies();
}

void EnemyManager::render(sf::RenderWindow& window) {
    for (auto& enemy : enemies) {
        if (enemy) {
            enemy->draw(window);
        }
    }
}

void EnemyManager::spawnEnemy(float playerX) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sideDist(0, 1); // 0 for left, 1 for right
    std::uniform_real_distribution<float> offsetDist(100.0f, 300.0f);
    
    // Decide which side to spawn on
    bool spawnOnRight = sideDist(gen) == 1;
    
    // Calculate spawn position
    float spawnX;
    if (spawnOnRight) {
        spawnX = playerX + 1280.0f / 2.0f + offsetDist(gen); // Right side of screen
    } else {
        spawnX = playerX - 1280.0f / 2.0f - offsetDist(gen); // Left side of screen
    }
    
    // Create and initialize new enemy
    Enemy* newEnemy = new Enemy();
    newEnemy->init(spawnX, spawnOnRight);
    enemies.push_back(newEnemy);
}

void EnemyManager::removeDeadEnemies() {
    for (size_t i = 0; i < enemies.size(); /* no increment */) {
        if (enemies[i]->canBeRemoved()) {
            delete enemies[i];
            enemies.erase(enemies.begin() + i);
        } else {
            ++i;
        }
    }
}

void EnemyManager::increaseWave() {
    currentWaveNumber++;
    // Make spawning faster as waves progress
    spawnInterval = std::max(0.5f, spawnInterval * 0.9f);
}

int EnemyManager::getCurrentWave() const {
    return currentWaveNumber;
}

int EnemyManager::getRemainingEnemies() const {
    // Count alive enemies
    int count = 0;
    for (const auto& enemy : enemies) {
        if (enemy && enemy->isAlive()) {
            count++;
        }
    }
    return count;
}

std::vector<Enemy*>& EnemyManager::getEnemies() {
    return enemies;
}

bool EnemyManager::isWaveCleared() const {
    return getRemainingEnemies() == 0 && 
           static_cast<int>(enemies.size()) >= enemiesPerWave * currentWaveNumber;
}
